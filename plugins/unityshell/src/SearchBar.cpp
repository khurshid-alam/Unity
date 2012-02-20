// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010-2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more detais.
 *
 * You shoud have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 */

#include "config.h"

#include <Nux/Nux.h>
#include <Nux/BaseWindow.h>
#include <Nux/HLayout.h>
#include <Nux/VLayout.h>
#include <Nux/Layout.h>
#include <Nux/WindowCompositor.h>
#include <NuxCore/Logger.h>

#include <NuxImage/CairoGraphics.h>
#include <NuxImage/ImageSurface.h>
#include <Nux/StaticText.h>

#include <NuxGraphics/GLThread.h>
#include <NuxGraphics/RenderingPipe.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "SearchBar.h"
#include <UnityCore/Variant.h>

#include "CairoTexture.h"
#include "DashStyle.h"

namespace
{
const float kExpandDefaultIconOpacity = 1.0f;
const int external_margin_vertical = 8;
const int external_margin_horizontal = 7;
const int LIVE_SEARCH_TIMEOUT = 40;
const int SPINNER_TIMEOUT = 100;

// Highlight
const int HIGHLIGHT_HEIGHT = 24;
const int HIGHLIGHT_WIDTH = 292;
const int HIGHLIGHT_LEFT_PADDING = 5;
const int HIGHLIGHT_RIGHT_PADDING = 4;
}

namespace
{

nux::logging::Logger logger("unity");
  
class ExpanderView : public nux::View
{
public:
  ExpanderView(NUX_FILE_LINE_DECL)
   : nux::View(NUX_FILE_LINE_PARAM)
  {
  }

protected:
  void Draw(nux::GraphicsEngine& graphics_engine, bool force_draw)
  {
  };

  void DrawContent(nux::GraphicsEngine& graphics_engine, bool force_draw)
  {
    if (GetLayout())
      GetLayout()->ProcessDraw(graphics_engine, force_draw);
  }

  bool AcceptKeyNavFocus()
  {
    return false;
  }
};

}

namespace unity
{

NUX_IMPLEMENT_OBJECT_TYPE(SearchBar);

SearchBar::SearchBar(NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  , search_hint("")
  , showing_filters(false)
  , can_refine_search(false)
  , disable_glow(false)
  , show_filter_hint_(true)
  , expander_view_(nullptr)
  , search_bar_width_(642)
  , live_search_timeout_(0)
  , start_spinner_timeout_(0)
{
  Init();
}

SearchBar::SearchBar(int search_bar_width, bool show_filter_hint_, NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  , search_hint("")
  , showing_filters(false)
  , can_refine_search(false)
  , disable_glow(false)
  , show_filter_hint_(show_filter_hint_)
  , expander_view_(nullptr)
  , search_bar_width_(search_bar_width)
  , live_search_timeout_(0)
  , start_spinner_timeout_(0)
{
  Init();
}

SearchBar::SearchBar(int search_bar_width, NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  , search_hint("")
  , showing_filters(false)
  , can_refine_search(false)
  , disable_glow(false)
  , show_filter_hint_(true)
  , expander_view_(nullptr)
  , search_bar_width_(search_bar_width)
  , live_search_timeout_(0)
  , start_spinner_timeout_(0)
{
  Init();
}

void SearchBar::Init()
{
  nux::BaseTexture* icon = dash::Style::Instance().GetSearchMagnifyIcon();

  bg_layer_ = new nux::ColorLayer(nux::Color(0xff595853), true);

  layout_ = new nux::HLayout(NUX_TRACKER_LOCATION);
  layout_->SetHorizontalInternalMargin(0);
  layout_->SetVerticalExternalMargin(external_margin_vertical);
  layout_->SetHorizontalExternalMargin(external_margin_horizontal);
  SetLayout(layout_);

  spinner_ = new SearchBarSpinner();
  spinner_->SetMinMaxSize(icon->GetWidth(), icon->GetHeight());
  spinner_->mouse_click.connect(sigc::mem_fun(this, &SearchBar::OnClearClicked));
  layout_->AddView(spinner_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  hint_ = new nux::StaticCairoText(" ");
  hint_->SetTextColor(nux::Color(1.0f, 1.0f, 1.0f, 0.5f));
  hint_->SetMaximumWidth(search_bar_width_ - icon->GetWidth());

  pango_entry_ = new IMTextEntry();
  pango_entry_->sigTextChanged.connect(sigc::mem_fun(this, &SearchBar::OnSearchChanged));
  pango_entry_->activated.connect([&]() { activated.emit(); });
  pango_entry_->cursor_moved.connect([&](int i) { QueueDraw(); });
  pango_entry_->mouse_down.connect(sigc::mem_fun(this, &SearchBar::OnMouseButtonDown));
  pango_entry_->end_key_focus.connect(sigc::mem_fun(this, &SearchBar::OnEndKeyFocus));
  pango_entry_->SetMaximumWidth(search_bar_width_ - 1.5 * icon->GetWidth());

  layered_layout_ = new nux::LayeredLayout();
  layered_layout_->AddLayer(hint_);
  layered_layout_->AddLayer(pango_entry_);
  layered_layout_->SetPaintAll(true);
  layered_layout_->SetActiveLayerN(1);
  layered_layout_->SetMinimumWidth(search_bar_width_);
  layered_layout_->SetMaximumWidth(search_bar_width_);
  layout_->AddView(layered_layout_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FIX);

  if (show_filter_hint_)
  {
    std::string filter_str(_("<small><b>Filter results</b></small>"));
    show_filters_ = new nux::StaticCairoText(filter_str.c_str());
    show_filters_->SetVisible(false);
    show_filters_->SetFont("Ubuntu 10");
    show_filters_->SetTextColor(nux::Color(1.0f, 1.0f, 1.0f, 1.0f));
    show_filters_->SetTextAlignment(nux::StaticCairoText::NUX_ALIGN_LEFT);

    nux::BaseTexture* arrow;
    arrow = dash::Style::Instance().GetGroupExpandIcon();
    expand_icon_ = new IconTexture(arrow,
                                   arrow->GetWidth(),
                                   arrow->GetHeight());
    expand_icon_->SetOpacity(kExpandDefaultIconOpacity);
    expand_icon_->SetMinimumSize(arrow->GetWidth(), arrow->GetHeight());
    expand_icon_->SetVisible(false);

    filter_layout_ = new nux::HLayout();
    filter_layout_->SetHorizontalInternalMargin(8);
    filter_layout_->SetHorizontalExternalMargin(6);
    filter_layout_->AddView(show_filters_, 0, nux::MINOR_POSITION_CENTER);

    arrow_layout_  = new nux::VLayout();
    arrow_top_space_ = new nux::SpaceLayout(2, 2, 12, 12);
    arrow_bottom_space_ = new nux::SpaceLayout(2, 2, 8, 8);
    arrow_layout_->AddView(arrow_top_space_, 0, nux::MINOR_POSITION_CENTER);
    arrow_layout_->AddView(expand_icon_, 0, nux::MINOR_POSITION_CENTER);
    arrow_layout_->AddView(arrow_bottom_space_, 0, nux::MINOR_POSITION_CENTER);

    filter_layout_->AddView(arrow_layout_, 0, nux::MINOR_POSITION_CENTER);

    layout_->AddLayout(new nux::SpaceLayout(1, 10000, 0, 1), 1);

    expander_view_ = new ExpanderView(NUX_TRACKER_LOCATION);
    expander_view_->SetVisible(false);
    expander_view_->SetLayout(filter_layout_);
    layout_->AddView(expander_view_, 0, nux::MINOR_POSITION_RIGHT, nux::MINOR_SIZE_FULL);

    // Lambda functions
    auto mouse_redraw = [&](int x, int y, unsigned long b, unsigned long k)
    {
      QueueDraw();
    };

    auto mouse_expand = [&](int x, int y, unsigned long b, unsigned long k)
    {
      showing_filters = !showing_filters;
    };

    // Signals
    expander_view_->mouse_click.connect(mouse_expand);
    expander_view_->mouse_enter.connect(mouse_redraw);
    expander_view_->mouse_leave.connect(mouse_redraw);
    show_filters_->mouse_click.connect(mouse_expand);
    show_filters_->mouse_enter.connect(mouse_redraw);
    show_filters_->mouse_leave.connect(mouse_redraw);
    expand_icon_->mouse_click.connect(mouse_expand);
    expand_icon_->mouse_enter.connect(mouse_redraw);
    expand_icon_->mouse_leave.connect(mouse_redraw);
    }

  sig_manager_.Add(new Signal<void, GtkSettings*, GParamSpec*>
      (gtk_settings_get_default(),
       "notify::gtk-font-name",
       sigc::mem_fun(this, &SearchBar::OnFontChanged)));
  OnFontChanged(gtk_settings_get_default());

  search_hint.changed.connect([&](std::string const& s) { OnSearchHintChanged(); });
  search_string.SetGetterFunction(sigc::mem_fun(this, &SearchBar::get_search_string));
  search_string.SetSetterFunction(sigc::mem_fun(this, &SearchBar::set_search_string));
  im_active.SetGetterFunction(sigc::mem_fun(this, &SearchBar::get_im_active));
  showing_filters.changed.connect(sigc::mem_fun(this, &SearchBar::OnShowingFiltersChanged));
  can_refine_search.changed.connect([&] (bool can_refine)
  {
    if (show_filter_hint_)
    {
      expander_view_->SetVisible(can_refine);
      show_filters_->SetVisible(can_refine);
      expand_icon_->SetVisible(can_refine);
    }
  });

  disable_glow.changed.connect([&](bool disabled)
  {
    layout_->SetVerticalExternalMargin(0);
    layout_->SetHorizontalExternalMargin(0);
    UpdateBackground(true);
    QueueDraw();
  });

}

SearchBar::~SearchBar()
{
  delete bg_layer_;

  if (live_search_timeout_)
    g_source_remove(live_search_timeout_);

  if (start_spinner_timeout_)
    g_source_remove(start_spinner_timeout_);
}

void SearchBar::OnFontChanged(GtkSettings* settings, GParamSpec* pspec)
{
  static const int HOW_LARGE = 8;
  gchar* font_name = NULL;
  PangoFontDescription* desc;
  gint size;
  gchar* font_desc;

  g_object_get(settings, "gtk-font-name", &font_name, NULL);

  desc = pango_font_description_from_string(font_name);
  pango_entry_->SetFontFamily(pango_font_description_get_family(desc));

  size = pango_font_description_get_size(desc);
  size /= pango_font_description_get_size_is_absolute(desc) ? 1 : PANGO_SCALE;
  pango_entry_->SetFontSize(size + HOW_LARGE);

  pango_entry_->SetFontOptions(gdk_screen_get_font_options(gdk_screen_get_default()));

  font_desc = g_strdup_printf("%s %d", pango_font_description_get_family(desc), size + HOW_LARGE);
  hint_->SetFont(font_desc);

  g_free(font_desc);
  font_desc = g_strdup_printf("%s %d", pango_font_description_get_family(desc), size + HOW_LARGE/2);
  show_filters_->SetFont(font_desc);

  pango_font_description_free(desc);
  g_free(font_name);
  g_free(font_desc);
}

void SearchBar::OnSearchHintChanged()
{
  std::string hint = search_hint;
  gchar* tmp = g_markup_escape_text(hint.c_str(), -1);

  gchar* markup  = g_strdup_printf("<span font_size='small' font_style='italic'> %s </span>", tmp);
  hint_->SetText(markup);

  g_free(markup);
  g_free(tmp);
}

void SearchBar::OnSearchChanged(nux::TextEntry* text_entry)
{
  // We don't want to set a new search string on every new character, so we add a sma
  // timeout to see if the user is typing a sentence. If more characters are added, we
  // keep restarting the timeout unti the user has actuay paused.
  if (live_search_timeout_)
    g_source_remove(live_search_timeout_);

  live_search_timeout_ = g_timeout_add(LIVE_SEARCH_TIMEOUT,
                                       (GSourceFunc)&OnLiveSearchTimeout,
                                       this);

  // Don't animate the spinner immediately, the searches are fast and
  // the spinner would just flicker
  if (start_spinner_timeout_)
    g_source_remove(start_spinner_timeout_);

  start_spinner_timeout_ = g_timeout_add(SPINNER_TIMEOUT,
                                         (GSourceFunc)&OnSpinnerStartCb,
                                         this);

  bool is_empty = pango_entry_->im_active() ? false : pango_entry_->GetText() == "";
  hint_->SetVisible(is_empty);

  pango_entry_->QueueDraw();
  hint_->QueueDraw();
  QueueDraw();

  search_changed.emit(pango_entry_->GetText());
}

gboolean SearchBar::OnLiveSearchTimeout(SearchBar* sef)
{
  sef->live_search_reached.emit(sef->pango_entry_->GetText());
  sef->live_search_timeout_ = 0;

  return FALSE;
}

gboolean SearchBar::OnSpinnerStartCb(SearchBar* sef)
{
  sef->spinner_->SetState(STATE_SEARCHING);
  sef->start_spinner_timeout_ = 0;

  return FALSE;
}

void SearchBar::OnShowingFiltersChanged(bool is_showing)
{
  if (show_filter_hint_)
  {
    dash::Style& style = dash::Style::Instance();
    if (is_showing)
      expand_icon_->SetTexture(style.GetGroupUnexpandIcon());
    else
      expand_icon_->SetTexture(style.GetGroupExpandIcon());
  }
}

void SearchBar::Draw(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  nux::Geometry const& base = GetGeometry();

  UpdateBackground(false);

  GfxContext.PushClippingRectangle(base);
  nux::GetPainter().PaintBackground(GfxContext, base);

  bg_layer_->SetGeometry(nux::Geometry(base.x, base.y, last_width_, base.height));
  nux::GetPainter().RenderSinglePaintLayer(GfxContext,
                                           bg_layer_->GetGeometry(),
                                           bg_layer_);

  if (ShouldBeHighlighted())
  {
    nux::Geometry geo(show_filters_->GetGeometry());
    nux::Geometry const& geo_arrow = arrow_layout_->GetGeometry();

    geo.y -= (HIGHLIGHT_HEIGHT- geo.height) / 2;
    geo.height = HIGHLIGHT_HEIGHT;
    geo.width = HIGHLIGHT_WIDTH + HIGHLIGHT_LEFT_PADDING + HIGHLIGHT_RIGHT_PADDING;
    geo.x = geo_arrow.x + (geo_arrow.width - 1) - geo.width + HIGHLIGHT_RIGHT_PADDING; 

    if (!highlight_layer_)
      highlight_layer_.reset(dash::Style::Instance().FocusOverlay(geo.width, geo.height));

    highlight_layer_->SetGeometry(geo);
    highlight_layer_->Renderlayer(GfxContext);
  }

  GfxContext.PopClippingRectangle();
}

void SearchBar::DrawContent(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  nux::Geometry const& geo = GetGeometry();

  GfxContext.PushClippingRectangle(geo);

  if (highlight_layer_ && ShouldBeHighlighted() && !IsFullRedraw())
  {
    nux::GetPainter().PushLayer(GfxContext, highlight_layer_->GetGeometry(), highlight_layer_.get());
  }


  if (!IsFullRedraw())
  {
    gPainter.PushLayer(GfxContext, bg_layer_->GetGeometry(), bg_layer_);
  }
  else
  {
    nux::GetPainter().PushPaintLayerStack();
  }

  layout_->ProcessDraw(GfxContext, force_draw);

  if (!IsFullRedraw())
  {
    gPainter.PopBackground();
  }
  else
  {
    nux::GetPainter().PopPaintLayerStack();
  }

  GfxContext.PopClippingRectangle();
}

void SearchBar::OnClearClicked(int x, int y, unsigned long button_fags,
                                     unsigned long key_fags)
{
  pango_entry_->SetText("");
  if (start_spinner_timeout_)
  {
    g_source_remove(start_spinner_timeout_);
    start_spinner_timeout_ = 0;
  }
  live_search_reached.emit("");
}

void SearchBar::OnEntryActivated()
{
  activated.emit();
}

void
SearchBar::SearchFinished()
{
  if (start_spinner_timeout_)
  {
    g_source_remove(start_spinner_timeout_);
    start_spinner_timeout_ = 0;
  }

  bool is_empty = pango_entry_->im_active() ?
    false : pango_entry_->GetText() == "";
  spinner_->SetState(is_empty ? STATE_READY : STATE_CLEAR);
}

void SearchBar::UpdateBackground(bool force)
{
  int PADDING = 12;
  int RADIUS = 5;
  int x, y, width, height;
  nux::Geometry geo(GetGeometry());
  geo.width = layered_layout_->GetGeometry().width;

  LOG_DEBUG(logger) << "height: "
  << geo.height << " - "
  << layered_layout_->GetGeometry().height << " - "
  << pango_entry_->GetGeometry().height;

  if (geo.width == last_width_
      && geo.height == last_height_
      && force == false)
    return;

  last_width_ = geo.width;
  last_height_ = geo.height;

  if (disable_glow)
    PADDING = 2;

  x = y = PADDING - 1;

  width = last_width_ - (2 * PADDING);
  height = last_height_ - (2 * PADDING) + 1;

  nux::CairoGraphics cairo_graphics(CAIRO_FORMAT_ARGB32, last_width_, last_height_);
  cairo_t* cr = cairo_graphics.GetContext();

  cairo_graphics.DrawRoundedRectangle(cr,
                                      1.0f,
                                      x,
                                      y,
                                      RADIUS,
                                      width,
                                      height,
                                      true);

  // Disable glow effect #929183
  //cairo_set_source_rgba(cr, 1.0f, 1.0f, 1.0f, 1.0f);
  //cairo_set_line_width(cr, 1.0);
  //cairo_stroke_preserve(cr);
  //cairo_graphics.BlurSurface (3, cairo_get_target (cr));

  // XXX: Not sure this code is 100% correct.
  cairo_operator_t op = CAIRO_OPERATOR_OVER;
  op = cairo_get_operator (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_set_source_rgba(cr, 0.0f, 0.0f, 0.0f, 0.35f);
  cairo_fill_preserve(cr);
  cairo_set_operator (cr, op);
  cairo_set_source_rgba(cr, 0.0f, 0.0f, 0.0f, 0.35f);
  cairo_fill_preserve(cr);
  cairo_set_source_rgba(cr, 1.0f, 1.0f, 1.0f, 0.8f);
  cairo_set_line_width(cr, 1.0);
  cairo_stroke(cr);

  cairo_destroy(cr);
  nux::BaseTexture* texture2D = texture_from_cairo_graphics(cairo_graphics);

  nux::TexCoordXForm texxform;
  texxform.SetTexCoordType(nux::TexCoordXForm::OFFSET_COORD);
  texxform.SetWrap(nux::TEXWRAP_REPEAT, nux::TEXWRAP_REPEAT);
  if (bg_layer_)
    delete bg_layer_;

  nux::ROPConfig rop;
  rop.Blend = true;
  rop.SrcBlend = GL_ONE;
  rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;

  bg_layer_ = new nux::TextureLayer(texture2D->GetDeviceTexture(),
                                    texxform,
                                    nux::color::White,
                                    true,
                                    rop);

  texture2D->UnReference();
}

void SearchBar::OnMouseButtonDown(int x, int y, unsigned long button, unsigned long key)
{
  search_hint = "";
}

void SearchBar::OnEndKeyFocus()
{
  search_hint = _("Search");
}

nux::TextEntry* SearchBar::text_entry() const
{
  return pango_entry_;
}

std::string SearchBar::get_search_string() const
{
  return pango_entry_->GetText();
}

bool SearchBar::set_search_string(std::string const& string)
{
  pango_entry_->SetText(string.c_str());
  spinner_->SetState(string == "" ? STATE_READY : STATE_CLEAR);

  // we don't want the spinner animating in this case
  if (start_spinner_timeout_)
  {
    g_source_remove(start_spinner_timeout_);
    start_spinner_timeout_ = 0;
  }

  return true;
}

bool SearchBar::get_im_active() const
{
  return pango_entry_->im_active();
}

//
// Highlight
//
bool SearchBar::ShouldBeHighlighted()
{
  return ((expander_view_ && expander_view_->IsVisible() && expander_view_->IsMouseInside()) ||
          (show_filters_ && show_filters_->IsVisible() && show_filters_->IsMouseInside()) ||
          (expand_icon_ && expand_icon_->IsVisible() && expand_icon_->IsMouseInside()));
}

//
// Key navigation
//
bool SearchBar::AcceptKeyNavFocus()
{
  return false;
}

//
// Introspection
//
std::string SearchBar::GetName() const
{
  return "SearchBar";
}

void SearchBar::AddProperties(GVariantBuilder* builder)
{
  unity::variant::BuilderWrapper wrapper(builder);

  wrapper.add(GetAbsoluteGeometry());
  wrapper.add("has_focus", pango_entry_->HasKeyFocus());
  wrapper.add("search_string", pango_entry_->GetText());
}

} // namespace unity