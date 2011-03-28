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
 * Authored by: Alejandro Piñeiro Iglesias <apinheiro@igalia.com>
 */

/**
 * SECTION:unity-launcher-accessible
 * @Title: UnityLauncherAccessible
 * @short_description: Implementation of the ATK interfaces for #Launcher
 * @see_also: Launcher
 *
 * #UnityLauncherAccessible implements the required ATK interfaces for
 * #Launcher, ie: exposing the different LauncherIcon on the model as
 * #child of the object.
 *
 */

#include <glib/gi18n.h>

#include "unity-launcher-accessible.h"
#include "unity-launcher-icon-accessible.h"

#include "unitya11y.h"
#include "Launcher.h"
#include "LauncherModel.h"

/* GObject */
static void unity_launcher_accessible_class_init (UnityLauncherAccessibleClass *klass);
static void unity_launcher_accessible_init       (UnityLauncherAccessible *self);
static void unity_launcher_accessible_finalize   (GObject *object);

/* AtkObject.h */
static void       unity_launcher_accessible_initialize     (AtkObject *accessible,
                                                            gpointer   data);
static gint       unity_launcher_accessible_get_n_children (AtkObject *obj);
static AtkObject *unity_launcher_accessible_ref_child      (AtkObject *obj,
                                                            gint i);
static AtkStateSet *unity_launcher_accessible_ref_state_set  (AtkObject *obj);

/* AtkSelection */
static void       atk_selection_interface_init                  (AtkSelectionIface *iface);
static AtkObject* unity_launcher_accessible_ref_selection       (AtkSelection *selection,
                                                                 gint i);
static gint       unity_launcher_accessible_get_selection_count (AtkSelection *selection);
static gboolean   unity_launcher_accessible_is_child_selected   (AtkSelection *selection,
                                                                 gint i);

/* private */
static void on_selection_change_cb (UnityLauncherAccessible *launcher_accessible);
static void on_icon_added_cb       (LauncherIcon *icon, UnityLauncherAccessible *self);
static void on_icon_removed_cb     (LauncherIcon *icon, UnityLauncherAccessible *self);
static void on_order_change_cb     (UnityLauncherAccessible *self);
static void update_children_index  (UnityLauncherAccessible *self);

G_DEFINE_TYPE_WITH_CODE (UnityLauncherAccessible, unity_launcher_accessible,  NUX_TYPE_VIEW_ACCESSIBLE,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_SELECTION, atk_selection_interface_init))

#define UNITY_LAUNCHER_ACCESSIBLE_GET_PRIVATE(obj)                      \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), UNITY_TYPE_LAUNCHER_ACCESSIBLE,  \
                                UnityLauncherAccessiblePrivate))

struct _UnityLauncherAccessiblePrivate
{
  sigc::connection on_selection_change_connection;
  sigc::connection on_icon_added_connection;
  sigc::connection on_icon_removed_connection;
  sigc::connection on_order_changed_connection;

  gboolean focused;
};


static void
unity_launcher_accessible_class_init (UnityLauncherAccessibleClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (klass);

  gobject_class->finalize = unity_launcher_accessible_finalize;

  /* AtkObject */
  atk_class->get_n_children = unity_launcher_accessible_get_n_children;
  atk_class->ref_child = unity_launcher_accessible_ref_child;
  atk_class->initialize = unity_launcher_accessible_initialize;
  atk_class->ref_state_set = unity_launcher_accessible_ref_state_set;

  g_type_class_add_private (gobject_class, sizeof (UnityLauncherAccessiblePrivate));
}

static void
unity_launcher_accessible_init (UnityLauncherAccessible *self)
{
  UnityLauncherAccessiblePrivate *priv =
    UNITY_LAUNCHER_ACCESSIBLE_GET_PRIVATE (self);

  self->priv = priv;
}

static void
unity_launcher_accessible_finalize (GObject *object)
{
  UnityLauncherAccessible *self = UNITY_LAUNCHER_ACCESSIBLE (object);

  self->priv->on_selection_change_connection.disconnect ();
  self->priv->on_icon_added_connection.disconnect ();
  self->priv->on_icon_removed_connection.disconnect ();
  self->priv->on_order_changed_connection.disconnect ();

  G_OBJECT_CLASS (unity_launcher_accessible_parent_class)->finalize (object);
}

AtkObject*
unity_launcher_accessible_new (nux::Object *object)
{
  AtkObject *accessible = NULL;

  g_return_val_if_fail (dynamic_cast<Launcher *>(object), NULL);

  accessible = ATK_OBJECT (g_object_new (UNITY_TYPE_LAUNCHER_ACCESSIBLE, NULL));

  atk_object_initialize (accessible, object);
  atk_object_set_name (accessible, _("LauncherAccessible"));

  return accessible;
}

/* AtkObject.h */
static void
unity_launcher_accessible_initialize (AtkObject *accessible,
                                      gpointer data)
{
  Launcher *launcher = NULL;
  nux::Object *nux_object = NULL;
  UnityLauncherAccessible *self = NULL;
  LauncherModel *model = NULL;

  ATK_OBJECT_CLASS (unity_launcher_accessible_parent_class)->initialize (accessible, data);

  accessible->role = ATK_ROLE_TOOL_BAR;

  self = UNITY_LAUNCHER_ACCESSIBLE (accessible);
  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (accessible));

  launcher = dynamic_cast<Launcher *>(nux_object);

  self->priv->on_selection_change_connection  =
    launcher->selection_change.connect (sigc::bind (sigc::ptr_fun (on_selection_change_cb), self));

  model = launcher->GetModel ();

  if (model)
    {
      self->priv->on_icon_added_connection =
        model->icon_added.connect (sigc::bind (sigc::ptr_fun (on_icon_added_cb), self));

      self->priv->on_icon_removed_connection =
        model->icon_removed.connect (sigc::bind (sigc::ptr_fun (on_icon_removed_cb), self));

      self->priv->on_order_changed_connection =
        model->order_changed.connect (sigc::bind (sigc::ptr_fun (on_order_change_cb), self));
    }
}

static gint
unity_launcher_accessible_get_n_children (AtkObject *obj)
{
  nux::Object *object = NULL;
  Launcher *launcher = NULL;
  LauncherModel *launcher_model = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (obj), 0);

  object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (obj));
  if (!object) /* state is defunct */
    return 0;

  launcher = dynamic_cast<Launcher *>(object);

  launcher_model = launcher->GetModel ();

  if (launcher_model)
    return launcher_model->Size ();
  else
    return 0;
}

static AtkObject*
unity_launcher_accessible_ref_child (AtkObject *obj,
                                     gint i)
{
  gint num = 0;
  nux::Object *nux_object = NULL;
  Launcher *launcher = NULL;
  LauncherModel *launcher_model = NULL;
  LauncherModel::iterator it;
  nux::Object *child = NULL;
  AtkObject *child_accessible = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (obj), NULL);
  num = atk_object_get_n_accessible_children (obj);
  g_return_val_if_fail ((i < num)&&(i >= 0), NULL);

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (obj));
  if (!nux_object) /* state is defunct */
    return 0;

  launcher = dynamic_cast<Launcher *>(nux_object);

  launcher_model = launcher->GetModel ();

  it = launcher_model->begin ();
  std::advance (it, i);

  child = dynamic_cast<nux::Object *>(*it);
  child_accessible = unity_a11y_get_accessible (child);

  g_debug ("[a11y][launcher] ref_child (%p:%s)",
           child_accessible, atk_object_get_name (child_accessible));

  g_object_ref (child_accessible);

  return child_accessible;
}

static AtkStateSet*
unity_launcher_accessible_ref_state_set (AtkObject *obj)
{
  AtkStateSet *state_set = NULL;
  nux::Object *nux_object = NULL;
  UnityLauncherAccessible *self = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (obj), NULL);

  state_set =
    ATK_OBJECT_CLASS (unity_launcher_accessible_parent_class)->ref_state_set (obj);

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (obj));

  if (nux_object == NULL) /* actor is defunct */
    return state_set;

  self = UNITY_LAUNCHER_ACCESSIBLE (obj);

  /* The Launcher is always focusable */
  atk_state_set_add_state (state_set, ATK_STATE_FOCUSABLE);

  if (self->priv->focused)
    atk_state_set_add_state (state_set, ATK_STATE_FOCUSED);

  return state_set;
}

/* AtkSelection */
static void
atk_selection_interface_init (AtkSelectionIface *iface)
{
  iface->ref_selection = unity_launcher_accessible_ref_selection;
  iface->get_selection_count = unity_launcher_accessible_get_selection_count;
  iface->is_child_selected = unity_launcher_accessible_is_child_selected;

  /* NOTE: for the moment we don't provide the implementation for the
     "interactable" methods, it is, the methods that allow to change
     the selected icon. The Launcher doesn't provide that API, and
     right now  we are focusing on a normal user input.*/
  /* iface->add_selection = unity_launcher_accessible_add_selection; */
  /* iface->clear_selection = unity_launcher_accessible_clear_selection; */
  /* iface->remove_selection = unity_launcher_accessible_remove_selection; */

  /* This method will never be implemented, as select all the launcher
     icons makes no sense */
  /* iface->select_all = unity_launcher_accessible_select_all_selection; */
}

static AtkObject*
unity_launcher_accessible_ref_selection (AtkSelection *selection,
                                         gint i)
{
  Launcher *launcher = NULL;
  LauncherIcon *selected_icon = NULL;
  nux::Object *nux_object = NULL;
  AtkObject *accessible_selected = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (selection), 0);
  /* there can be only just item selected */
  g_return_val_if_fail (i == 0, NULL);

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (selection));
  if (!nux_object) /* state is defunct */
    return 0;

  launcher = dynamic_cast<Launcher *>(nux_object);
  selected_icon = launcher->GetSelectedMenuIcon ();

  if (selected_icon != 0)
    {
      accessible_selected = unity_a11y_get_accessible (selected_icon);
      g_object_ref (accessible_selected);
    }

  return accessible_selected;
}

static gint
unity_launcher_accessible_get_selection_count (AtkSelection *selection)
{
  Launcher *launcher = NULL;
  LauncherIcon *selected_icon = NULL;
  nux::Object *nux_object = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (selection), 0);

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (selection));
  if (!nux_object) /* state is defunct */
    return 0;

  launcher = dynamic_cast<Launcher *>(nux_object);
  selected_icon = launcher->GetSelectedMenuIcon ();

  if (selected_icon == 0)
    return 0;
  else
    return 1;
}

static gboolean
unity_launcher_accessible_is_child_selected (AtkSelection *selection,
                                             gint i)
{
  Launcher *launcher = NULL;
  LauncherIcon *icon = NULL;
  LauncherIcon *selected_icon = NULL;
  LauncherModel *launcher_model = NULL;
  LauncherModel::iterator it;
  nux::Object *nux_object = NULL;

  g_return_val_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (selection), FALSE);

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (selection));
  if (!nux_object) /* state is defunct */
    return 0;

  launcher = dynamic_cast<Launcher *>(nux_object);
  launcher_model = launcher->GetModel ();
  it = launcher_model->begin ();
  std::advance (it, i);
  icon = dynamic_cast<LauncherIcon *>(*it);

  selected_icon = launcher->GetSelectedMenuIcon ();

  if (selected_icon == icon)
    return TRUE;
  else
    return FALSE;
}

/* private */
static void on_selection_change_cb (UnityLauncherAccessible *launcher_accessible)
{
  g_debug ("[a11y][launcher] selection changed");

  g_signal_emit_by_name (ATK_OBJECT (launcher_accessible), "selection-changed");
}


static void
on_icon_added_cb (LauncherIcon *icon,
                  UnityLauncherAccessible *self)
{
  AtkObject *icon_accessible = NULL;
  nux::Object *nux_object = NULL;
  gint index = 0;

  g_return_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (self));

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (self));
  if (nux_object == NULL) /* state is defunct */
    return;

  icon_accessible = unity_a11y_get_accessible (icon);

  update_children_index (self);

  index = atk_object_get_index_in_parent (icon_accessible);

  g_debug ("[a11y][launcher] icon (%p, %s) added on container (%p,%s) at index %i",
           icon_accessible, atk_object_get_name (icon_accessible),
           self, atk_object_get_name ( ATK_OBJECT (self)),
           index);

  g_signal_emit_by_name (self, "children-changed::add",
                         index, icon_accessible, NULL);
}

static void
on_icon_removed_cb (LauncherIcon *icon,
                    UnityLauncherAccessible *self)
{
  AtkObject *icon_accessible = NULL;
  nux::Object *nux_object = NULL;
  gint index = 0;

  g_return_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (self));

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (self));
  if (nux_object == NULL) /* state is defunct */
    return;

  icon_accessible = unity_a11y_get_accessible (icon);

  index = atk_object_get_index_in_parent (icon_accessible);

  g_debug ("[a11y][launcher] icon (%p, %s) removed on container (%p,%s) at index %i",
           icon_accessible, atk_object_get_name (icon_accessible),
           self, atk_object_get_name (ATK_OBJECT (self)),
           index);

  g_signal_emit_by_name (self, "children-changed::remove",
                         index, icon_accessible, NULL);

  update_children_index (self);
}

static void
update_children_index  (UnityLauncherAccessible *self)
{
  gint index = 0;
  nux::Object *nux_object = NULL;
  Launcher *launcher = NULL;
  LauncherModel *launcher_model = NULL;
  LauncherModel::iterator it;
  nux::Object *child = NULL;
  AtkObject *child_accessible = NULL;

  nux_object = nux_object_accessible_get_object (NUX_OBJECT_ACCESSIBLE (self));
  if (!nux_object) /* state is defunct */
    return;

  launcher = dynamic_cast<Launcher *>(nux_object);
  launcher_model = launcher->GetModel ();

  if (launcher_model == NULL)
    return;

  for (it = launcher_model->begin (); it != launcher_model->end (); it++)
    {
      child =  dynamic_cast<nux::Object *>(*it);
      child_accessible = unity_a11y_get_accessible (child);

      unity_launcher_icon_accessible_set_index (UNITY_LAUNCHER_ICON_ACCESSIBLE (child_accessible),
                                                index++);
    }
}

static void
on_order_change_cb (UnityLauncherAccessible *self)
{
  g_return_if_fail (UNITY_IS_LAUNCHER_ACCESSIBLE (self));

  update_children_index (self);
}
