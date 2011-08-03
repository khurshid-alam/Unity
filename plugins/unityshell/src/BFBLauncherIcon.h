// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by: Jason Smith <jason.smith@canonical.com>
 */

#ifndef BFBLAUNCHERICON_H
#define BFBLAUNCHERICON_H

#include "SimpleLauncherIcon.h"

class BFBLauncherIcon : public SimpleLauncherIcon
{

public:
  BFBLauncherIcon(Launcher* launcher);
  ~BFBLauncherIcon();

  virtual nux::Color BackgroundColor();
  virtual nux::Color GlowColor();

  void ActivateLauncherIcon(ActionArg arg);
};

#endif // BFBLAUNCHERICON_H
