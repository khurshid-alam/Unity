// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 */

#include "config.h"

#include <Nux/Nux.h>
#include <Nux/BaseWindow.h>
#include <Nux/HLayout.h>
#include <Nux/Layout.h>
#include <Nux/WindowCompositor.h>

#include <NuxImage/CairoGraphics.h>
#include <NuxImage/ImageSurface.h>

#include <NuxGraphics/GLThread.h>
#include <NuxGraphics/RenderingPipe.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "PlacesSearchBar.h"

#include "PlacesStyle.h"

#define LIVE_SEARCH_TIMEOUT 250

NUX_IMPLEMENT_OBJECT_TYPE (PlacesSearchBar);

PlacesSearchBar::PlacesSearchBar (NUX_FILE_LINE_DECL)
:   View (NUX_FILE_LINE_PARAM),
    _entry (NULL),
    _live_search_timeout (0)
{
  PlacesStyle      *style = PlacesStyle::GetDefault ();
  nux::BaseTexture *icon = style->GetSearchReadyIcon ();

  _bg_layer = new nux::ColorLayer (nux::Color (0xff595853), true);

  _layout = new nux::HLayout (NUX_TRACKER_LOCATION);
  _layout->SetHorizontalInternalMargin (12);

  _search_icon = new nux::TextureArea (NUX_TRACKER_LOCATION);
  _search_icon->SetTexture (icon);
  _search_icon->SetMinMaxSize (icon->GetWidth (), icon->GetHeight ());
  _layout->AddView (_search_icon, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  _search_icon->OnMouseClick.connect (sigc::mem_fun (this, &PlacesSearchBar::OnClearClicked));

  _layered_layout = new nux::LayeredLayout ();

  _hint = new nux::StaticCairoText (" ");
  _hint->SetTextColor (nux::Color (1.0f, 1.0f, 1.0f, 0.5f));
  _layered_layout->AddLayer (_hint);

  _pango_entry = new nux::TextEntry ("", NUX_TRACKER_LOCATION);
  _pango_entry->sigTextChanged.connect (sigc::mem_fun (this, &PlacesSearchBar::OnSearchChanged));
  _layered_layout->AddLayer (_pango_entry);

  _layered_layout->SetPaintAll (true);
  _layered_layout->SetActiveLayerN (1);

  _layout->AddView (_layered_layout, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FIX);
  
  _combo = new nux::ComboBoxSimple(NUX_TRACKER_LOCATION);
  _combo->SetMaximumWidth (style->GetHomeTileWidth ());
  _combo->SetVisible (false);
  _combo->sigTriggered.connect (sigc::mem_fun (this, &PlacesSearchBar::OnComboChanged));
  _layout->AddView (_combo, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FIX);

  _layout->SetVerticalExternalMargin (18);
  _layout->SetHorizontalExternalMargin (18);

  SetLayout (_layout);

  SetCompositionLayout (_layout);

  g_signal_connect (gtk_settings_get_default (), "notify::gtk-font-name",
                    G_CALLBACK (OnFontChanged), this);
  OnFontChanged (NULL, NULL, this);

}

PlacesSearchBar::~PlacesSearchBar ()
{
  if (_bg_layer)
    delete _bg_layer;
}

const gchar* PlacesSearchBar::GetName ()
{
	return "PlacesSearchBar";
}

const gchar *
PlacesSearchBar::GetChildsName ()
{
  return "";
}

bool
PlacesSearchBar::CanFocus ()
{
  return false;
}

void PlacesSearchBar::AddProperties (GVariantBuilder *builder)
{
  nux::Geometry geo = GetGeometry ();

  g_variant_builder_add (builder, "{sv}", "x", g_variant_new_int32 (geo.x));
  g_variant_builder_add (builder, "{sv}", "y", g_variant_new_int32 (geo.y));
  g_variant_builder_add (builder, "{sv}", "width", g_variant_new_int32 (geo.width));
  g_variant_builder_add (builder, "{sv}", "height", g_variant_new_int32 (geo.height));
}

long
PlacesSearchBar::ProcessEvent (nux::IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
  long ret = TraverseInfo;
  ret = _layout->ProcessEvent (ievent, ret, ProcessEventInfo);

  return ret;
}

void
PlacesSearchBar::Draw (nux::GraphicsEngine& GfxContext, bool force_draw)
{
  nux::Geometry geo = GetGeometry ();

  UpdateBackground ();

 GfxContext.PushClippingRectangle (geo);

  _bg_layer->SetGeometry (nux::Geometry (geo.x, geo.y, _last_width, geo.height));
  nux::GetPainter ().RenderSinglePaintLayer (GfxContext,
                                             _bg_layer->GetGeometry (),
                                             _bg_layer);

  GfxContext.PopClippingRectangle ();
}

void
PlacesSearchBar::DrawContent (nux::GraphicsEngine &GfxContext, bool force_draw)
{
  nux::Geometry geo = GetGeometry ();

  GfxContext.PushClippingRectangle (geo);

  gPainter.PushLayer (GfxContext, _bg_layer->GetGeometry (), _bg_layer);

  _layout->ProcessDraw (GfxContext, force_draw);

  gPainter.PopBackground ();
  GfxContext.PopClippingRectangle();
}

void
PlacesSearchBar::SetActiveEntry (PlaceEntry *entry,
                                 guint       section_id,
                                 const char *search_string,
                                 bool        ignore_search)
{
   std::map<gchar *, gchar *> hints;

   _entry = entry;

   _combo->RemoveAllItem ();
   _combo->SetVisible (false);

  if (_entry)
  {
    // i18n: This is for a dynamic place name i.e. "Search Files & Folders"
    const gchar *search_template = _("<span font_size='x-small' font_style='italic'>Search %s</span>");
    gchar       *res;
    gchar       *tmp;

    tmp = g_markup_escape_text (entry->GetName (), -1);
    res = g_strdup_printf (search_template, tmp);

    _hint->SetText (res);

    if (!ignore_search)
    {
      _entry->SetActiveSection (section_id);
      _entry->SetSearch (search_string ? search_string : "", hints);
    }

    _pango_entry->SetText (search_string ? search_string : "");

    _entry->ForeachSection (sigc::mem_fun (this, &PlacesSearchBar::OnSectionAdded));
    if (_combo->IsVisible ())
      _combo->SetSelectionIndex (section_id);

    g_free (res);
    g_free (tmp);
  }
  else
  {
    _pango_entry->SetText ("");
  }
}

void
PlacesSearchBar::OnSectionAdded (PlaceEntry *entry, PlaceEntrySection& section)
{
  char *tmp = g_markup_escape_text (section.GetName (), -1);

  _combo->AddItem (tmp);
  _combo->SetVisible (true);

  g_free (tmp);
}

void
PlacesSearchBar::OnComboChanged (nux::ComboBoxSimple *simple)
{
  _entry->SetActiveSection (_combo->GetSelectionIndex());
}

void
PlacesSearchBar::OnSearchChanged (nux::TextEntry *text_entry)
{
  PlacesStyle *style = PlacesStyle::GetDefault ();
  bool         is_empty;

  if (_live_search_timeout)
    g_source_remove (_live_search_timeout);

  _live_search_timeout = g_timeout_add (LIVE_SEARCH_TIMEOUT,
                                        (GSourceFunc)&OnLiveSearchTimeout,
                                        this);

  search_changed.emit (_pango_entry->GetText ().c_str ());



  is_empty = g_strcmp0 (_pango_entry->GetText ().c_str (), "") == 0;
  _hint->SetVisible (is_empty);
  _search_icon->SetTexture (is_empty ? style->GetSearchReadyIcon () : style->GetSearchClearIcon ());

  _hint->QueueDraw ();
  _pango_entry->QueueDraw ();
  QueueDraw ();
}

bool
PlacesSearchBar::OnLiveSearchTimeout (PlacesSearchBar *self)
{
  self->EmitLiveSearch ();

  return FALSE;
}

void
PlacesSearchBar::EmitLiveSearch ()
{
  if (_entry)
  {
    std::map<gchar *, gchar *> hints;

    _entry->SetSearch (_pango_entry->GetText ().c_str (), hints);
  }
  _live_search_timeout = 0;
}

void
PlacesSearchBar::OnClearClicked (int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  if (_pango_entry->GetText () != "")
  {
    _pango_entry->SetText ("");
    _search_icon->SetTexture (PlacesStyle::GetDefault ()->GetSearchReadyIcon ());
    EmitLiveSearch ();
  }
}

void
PlacesSearchBar::OnFontChanged (GObject *object, GParamSpec *pspec, PlacesSearchBar *self)
{
#define HOW_LARGE 8
  GtkSettings          *settings;
  gchar                *font_name = NULL;
  PangoFontDescription *desc;
  gint                  size;
  gchar                *font_desc;

  settings = gtk_settings_get_default ();
  g_object_get (settings, "gtk-font-name", &font_name, NULL);

  desc = pango_font_description_from_string (font_name);
  self->_pango_entry->SetFontFamily (pango_font_description_get_family (desc));

  size = pango_font_description_get_size (desc);
  size /= pango_font_description_get_size_is_absolute (desc) ? 1 : PANGO_SCALE;
  self->_pango_entry->SetFontSize ( size + HOW_LARGE);

  self->_pango_entry->SetFontOptions (gdk_screen_get_font_options (gdk_screen_get_default ()));

  font_desc = g_strdup_printf ("%s %d", pango_font_description_get_family (desc), size + HOW_LARGE);
  self->_hint->SetFont (font_desc);

  pango_font_description_free (desc);
  g_free (font_name);
  g_free (font_desc);
}

static void
draw_rounded_rect (cairo_t* cr,
                   double   aspect,
                   double   x,
                   double   y,
                   double   cornerRadius,
                   double   width,
                   double   height)
{
    double radius = cornerRadius / aspect;

    // top-left, right of the corner
    cairo_move_to (cr, x + radius, y);

    // top-right, left of the corner
    cairo_line_to (cr, x + width - radius, y);

    // top-right, below the corner
    cairo_arc (cr,
               x + width - radius,
               y + radius,
               radius,
               -90.0f * G_PI / 180.0f,
               0.0f * G_PI / 180.0f);

    // bottom-right, above the corner
    cairo_line_to (cr, x + width, y + height - radius);

    // bottom-right, left of the corner
    cairo_arc (cr,
               x + width - radius,
               y + height - radius,
               radius,
               0.0f * G_PI / 180.0f,
               90.0f * G_PI / 180.0f);

    // bottom-left, right of the corner
    cairo_line_to (cr, x + radius, y + height);

    // bottom-left, above the corner
    cairo_arc (cr,
               x + radius,
               y + height - radius,
               radius,
               90.0f * G_PI / 180.0f,
               180.0f * G_PI / 180.0f);

    // top-left, right of the corner
    cairo_arc (cr,
               x + radius,
               y + radius,
               radius,
               180.0f * G_PI / 180.0f,
               270.0f * G_PI / 180.0f);
  cairo_close_path (cr);
}

void
PlacesSearchBar::UpdateBackground ()
{
#define PADDING 14
#define RADIUS  6
  int x, y, width, height;
  nux::Geometry geo = GetGeometry ();
  nux::Geometry cgeo = _combo->GetGeometry ();

  if (_combo->IsVisible ())
    geo.width = cgeo.x;

  if (geo.width == _last_width && geo.height == _last_height)
    return;

  _last_width = geo.width;
  _last_height = geo.height;

  x = y = PADDING;
  width = _last_width - (2*PADDING);
  height = _last_height - (2*PADDING);

  nux::CairoGraphics cairo_graphics(CAIRO_FORMAT_ARGB32, _last_width, _last_height);
  cairo_t *cr = cairo_graphics.GetContext();
  cairo_translate (cr, 0.5, 0.5);
  cairo_set_line_width (cr, 1.0);

  draw_rounded_rect (cr, 1.0f, x, y, RADIUS, width, height);

  cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 0.75f);
  cairo_fill_preserve (cr);

  cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 0.8f);
  cairo_stroke (cr);

  //FIXME: This is until we get proper glow
  draw_rounded_rect (cr, 1.0f, x-1, y-1, RADIUS, width+2, height+2);
  cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 0.4f);
  cairo_stroke (cr);

  draw_rounded_rect (cr, 1.0f, x-2, y-2, RADIUS, width+4, height+4);
  cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 0.2f);
  cairo_stroke (cr);

  draw_rounded_rect (cr, 1.0f, x-3, y-3, RADIUS, width+6, height+6);
  cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 0.1f);
  cairo_stroke (cr);

  cairo_destroy (cr);

  nux::NBitmapData* bitmap =  cairo_graphics.GetBitmap();

  nux::BaseTexture* texture2D = nux::GetThreadGLDeviceFactory ()->CreateSystemCapableTexture ();
  texture2D->Update(bitmap);
  delete bitmap;

  nux::TexCoordXForm texxform;
  texxform.SetTexCoordType (nux::TexCoordXForm::OFFSET_COORD);
  texxform.SetWrap (nux::TEXWRAP_REPEAT, nux::TEXWRAP_REPEAT);
  if (_bg_layer)
    delete _bg_layer;

  nux::ROPConfig rop;
  rop.Blend = true;                      // Disable the blending. By default rop.Blend is false.
  rop.SrcBlend = GL_ONE;                  // Set the source blend factor.
  rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;  // Set the destination blend factor.

  _bg_layer = new nux::TextureLayer (texture2D->GetDeviceTexture(),
                                     texxform,          // The Oject that defines the texture wraping and coordinate transformation.
                                     nux::Color::White, // The color used to modulate the texture.
                                     true,              // Write the alpha value of the texture to the destination buffer.
                                     rop                // Use the given raster operation to set the blending when the layer is being rendered.
  );

  texture2D->UnReference ();

  QueueDraw ();
}
