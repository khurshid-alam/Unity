// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Gordon Allott <gord.allott@canonical.com>
 *
 */

#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include <Nux/Nux.h>
#include <Nux/View.h>
#include <dee.h>

#include <UnityCore/GLibSignal.h>
#include <UnityCore/Results.h>

#include "unity-shared/Introspectable.h"
#include "ResultRenderer.h"

namespace unity
{
namespace dash
{
class ResultView : public nux::View, public debug::Introspectable
{
public:
  NUX_DECLARE_OBJECT_TYPE(ResultView, nux::View);

  typedef std::vector<Result> ResultList;

  ResultView(NUX_FILE_LINE_DECL);
  virtual ~ResultView();

  void SetModelRenderer(ResultRenderer* renderer);

  void AddResult(Result& result);
  void RemoveResult(Result& result); 
  unsigned int GetIndexForUri(const std::string& uri) const; 

  ResultList GetResultList () const;

  nux::Property<bool> expanded;
  nux::Property<int> results_per_row;
  nux::Property<int> preview_spacer; // makes a vertical space for the preview with the value as the height
  nux::Property<std::string> preview_result_uri; //for highlighting a preview

  sigc::signal<void, std::string const&> UriActivated;

  std::string GetName() const;
  void AddProperties(GVariantBuilder* builder);
  IntrospectableList GetIntrospectableChildren();

protected:
  virtual void Draw(nux::GraphicsEngine& GfxContext, bool force_draw);
  virtual void DrawContent(nux::GraphicsEngine& GfxContext, bool force_draw);
  virtual long ComputeContentSize();

  static void ChildResultDestructor(debug::Introspectable* child);
  virtual debug::Introspectable* CreateResultWrapper(Result const& result, int index) const;

  // properties
  ResultRenderer* renderer_;
  ResultList results_;
  std::map<std::string, debug::Introspectable*> child_map_;
};

}
}

#endif //RESULTVIEW_H
