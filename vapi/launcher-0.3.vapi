/* launcher-0.3.vapi generated by vapigen, do not modify. */

[CCode (cprefix = "Launcher", lower_case_cprefix = "launcher_")]
namespace Launcher {
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Application : GLib.Object {
		[CCode (has_construct_function = false)]
		public Application ();
		public void add_wnckapp (Wnck.Application wnck_app);
		[CCode (has_construct_function = false)]
		public Application.from_desktop_file (string desktop_file);
		[CCode (has_construct_function = false)]
		public Application.from_wnck_app (Wnck.Application app);
		public unowned GLib.SList get_categories ();
		public unowned string get_comment ();
		public unowned string get_desktop_file ();
		public unowned string get_exec_string ();
		public bool get_favorite ();
		public unowned string get_icon_name ();
		public unowned string get_name ();
		public bool get_running ();
		public unowned string get_unique_string ();
		public unowned GLib.SList get_wnckapps ();
		public bool launch () throws GLib.Error;
		public void set_desktop_file (string desktop_file);
		public string comment { get; }
		[NoAccessorMethod]
		public string desktop_file_path { owned get; set; }
		[NoAccessorMethod]
		public string exec { owned get; }
		public bool favorite { get; }
		public string icon_name { get; }
		public string name { get; }
		public bool running { get; }
		public string unique_string { get; }
		public virtual signal void closed (Wnck.Application wnckapp);
		public virtual signal void opened (Wnck.Application wnckapp);
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Appman : GLib.Object {
		public unowned Launcher.Application get_application_for_desktop_file (string desktop);
		public unowned Launcher.Application get_application_for_wnck_app (Wnck.Application wnck_app);
		public unowned GLib.Sequence get_applications ();
		public static unowned Launcher.Appman get_default ();
	}
	[Compact]
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Category {
		public weak GLib.Object parent_instance;
		[CCode (has_construct_function = false)]
		public Category (string name, string comment, string icon_name);
		public void add_application (Launcher.Application application);
		public void empty_applications ();
		public unowned GLib.SList get_applications ();
		public unowned string get_comment ();
		public unowned string get_icon_name ();
		public unowned string get_name ();
		public void remove_application (Launcher.Application application);
		public void sort_applications (GLib.CompareFunc sort_func);
	}
	[Compact]
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class CategoryClass {
		public weak GLib.Callback application_added;
		public weak GLib.Callback application_removed;
		public weak GLib.ObjectClass parent_class;
		public weak GLib.Callback removed;
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Favorites : GLib.Object {
		public bool add_favorite (string uid);
		public bool get_bool (string uid, string key);
		public static unowned Launcher.Favorites get_default ();
		public unowned GLib.SList get_favorites ();
		public float get_float (string uid, string key);
		public int get_int (string uid, string key);
		public unowned GLib.SList get_list (string uid, string key, Launcher.FavoritesListValue value_type);
		public unowned string get_string (string uid, string key);
		public bool remove_favorite (string uid);
		public void set_bool (string uid, string key, bool value);
		public void set_favorites (GLib.SList list);
		public void set_float (string uid, string key, float value);
		public void set_int (string uid, string key, int value);
		public void set_list (string uid, string key, Launcher.FavoritesListValue value_type, GLib.SList value);
		public void set_string (string uid, string key, string value);
	}
	[Compact]
	[CCode (type_id = "LAUNCHER_TYPE_FOLDER", cheader_filename = "launcher/launcher.h")]
	public class Folder {
		[CCode (has_construct_function = false)]
		public Folder (string name, string uri);
		public void activate () throws GLib.Error;
		public unowned string get_icon_name ();
		public unowned string get_name ();
		public unowned string get_uri ();
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class FolderBookmarks : GLib.Object {
		public unowned GLib.SList get_bookmarks ();
		public static unowned Launcher.FolderBookmarks get_default ();
		public virtual signal void bookmarks_changed ();
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Menu : GLib.Object {
		public unowned GLib.SList get_applications ();
		public unowned GLib.SList get_categories ();
		public static unowned Launcher.Menu get_default ();
		public virtual signal void menu_changed ();
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class Session : GLib.Object {
		public static unowned Launcher.Session get_default ();
		public unowned GLib.SList get_running_applications ();
		public virtual signal void application_launching (void* p0);
		public virtual signal void application_opened (void* p0);
	}
	[Compact]
	[CCode (cheader_filename = "launcher/launcher.h")]
	public class SessionClass {
	}
	[CCode (cprefix = "LAUNCHER_FAVORITES_", has_type_id = "0", cheader_filename = "launcher/launcher.h")]
	public enum FavoritesListValue {
		STRING,
		INT,
		FLOAT,
		BOOL
	}
	[CCode (cheader_filename = "launcher/launcher.h")]
	public static unowned string icon_utils_icon_name_for_volume_uri (string uri);
}
