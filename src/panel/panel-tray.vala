/* -*- Mode: vala; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
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
 * Authored by Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

namespace Unity.Panel.Tray
{
  public class View : Ctk.Box
  {
    private TrayManager manager;
    private Clutter.Stage stage;

    public View ()
    {
      Object (orientation:Ctk.Orientation.HORIZONTAL);
    }

    construct
    {
      Clutter.Color color = { 0, 0, 0, 255 };
      this.manager = new TrayManager ();
      this.manager.tray_icon_added.connect (this.on_tray_icon_added);
      this.manager.tray_icon_removed.connect (this.on_tray_icon_removed);
    }

    public void manage_stage (Clutter.Stage stage)
    {
      this.stage = stage;
      //this.manager.manage_stage (stage);
      Idle.add (this.manage_tray_idle);
    }

    private bool manage_tray_idle ()
    {
      this.manager.manage_stage (this.stage);
      return false;
    }

    private void on_tray_icon_added (Clutter.Actor icon)
    {
      debug ("Icon added");
      this.add_actor (icon);
      icon.opacity = 100;
      icon.set_size (23, 23);
      icon.show ();
    }

    private void on_tray_icon_removed (Clutter.Actor icon)
    {
      debug ("Icon removed");
      this.remove_actor (icon);
    }
  }
}
