// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010-2012 Canonical Ltd
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

#ifndef UNITYSHARED_XWINDOW_MANAGER_H
#define UNITYSHARED_XWINDOW_MANAGER_H

#include <Nux/Nux.h>
#include <gdk/gdkx.h>
#include <core/core.h>

#include "unity-shared/WindowManager.h"

namespace unity
{

class XWindowManager : public WindowManager
{
public:
  XWindowManager();

  virtual void StartMove(Window window_id, int x, int y);

private:
  Atom move_resize_atom_;
};

}

#endif // WINDOW_MANAGER_H
