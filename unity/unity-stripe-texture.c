/* unity-stripe-texture.c generated by valac, the Vala compiler
 * generated from unity-stripe-texture.vala, do not modify */

/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3.0 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#include <glib.h>
#include <glib-object.h>
#include <clutk/clutk.h>
#include <clutter/clutter.h>
#include <cairo.h>
#include <float.h>
#include <math.h>


#define UNITY_TYPE_CAIRO_CANVAS (unity_cairo_canvas_get_type ())
#define UNITY_CAIRO_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNITY_TYPE_CAIRO_CANVAS, UnityCairoCanvas))
#define UNITY_CAIRO_CANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), UNITY_TYPE_CAIRO_CANVAS, UnityCairoCanvasClass))
#define UNITY_IS_CAIRO_CANVAS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNITY_TYPE_CAIRO_CANVAS))
#define UNITY_IS_CAIRO_CANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), UNITY_TYPE_CAIRO_CANVAS))
#define UNITY_CAIRO_CANVAS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), UNITY_TYPE_CAIRO_CANVAS, UnityCairoCanvasClass))

typedef struct _UnityCairoCanvas UnityCairoCanvas;
typedef struct _UnityCairoCanvasClass UnityCairoCanvasClass;
typedef struct _UnityCairoCanvasPrivate UnityCairoCanvasPrivate;

#define UNITY_TYPE_STRIPE_TEXTURE (unity_stripe_texture_get_type ())
#define UNITY_STRIPE_TEXTURE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNITY_TYPE_STRIPE_TEXTURE, UnityStripeTexture))
#define UNITY_STRIPE_TEXTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), UNITY_TYPE_STRIPE_TEXTURE, UnityStripeTextureClass))
#define UNITY_IS_STRIPE_TEXTURE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNITY_TYPE_STRIPE_TEXTURE))
#define UNITY_IS_STRIPE_TEXTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), UNITY_TYPE_STRIPE_TEXTURE))
#define UNITY_STRIPE_TEXTURE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), UNITY_TYPE_STRIPE_TEXTURE, UnityStripeTextureClass))

typedef struct _UnityStripeTexture UnityStripeTexture;
typedef struct _UnityStripeTextureClass UnityStripeTextureClass;
typedef struct _UnityStripeTexturePrivate UnityStripeTexturePrivate;
#define _cairo_surface_destroy0(var) ((var == NULL) ? NULL : (var = (cairo_surface_destroy (var), NULL)))
#define _cairo_destroy0(var) ((var == NULL) ? NULL : (var = (cairo_destroy (var), NULL)))
#define _cairo_pattern_destroy0(var) ((var == NULL) ? NULL : (var = (cairo_pattern_destroy (var), NULL)))

typedef void (*UnityCairoCanvasCairoCanvasPaint) (cairo_t* cr, gint width, gint height, void* user_data);
struct _UnityCairoCanvas {
	CtkBin parent_instance;
	UnityCairoCanvasPrivate * priv;
	ClutterCairoTexture* texture;
	UnityCairoCanvasCairoCanvasPaint paint_func;
	gpointer paint_func_target;
	GDestroyNotify paint_func_target_destroy_notify;
};

struct _UnityCairoCanvasClass {
	CtkBinClass parent_class;
};

typedef void (*UnityStripeTextureStripeTextureOutlineFunc) (cairo_t* cr, gint width, gint height, void* user_data);
struct _UnityStripeTexture {
	UnityCairoCanvas parent_instance;
	UnityStripeTexturePrivate * priv;
	UnityStripeTextureStripeTextureOutlineFunc outline_paint_func;
	gpointer outline_paint_func_target;
	GDestroyNotify outline_paint_func_target_destroy_notify;
};

struct _UnityStripeTextureClass {
	UnityCairoCanvasClass parent_class;
};

struct _UnityStripeTexturePrivate {
	float _radius;
	cairo_surface_t* pattern;
};


static gpointer unity_stripe_texture_parent_class = NULL;

GType unity_cairo_canvas_get_type (void) G_GNUC_CONST;
GType unity_stripe_texture_get_type (void) G_GNUC_CONST;
#define UNITY_STRIPE_TEXTURE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), UNITY_TYPE_STRIPE_TEXTURE, UnityStripeTexturePrivate))
enum  {
	UNITY_STRIPE_TEXTURE_DUMMY_PROPERTY,
	UNITY_STRIPE_TEXTURE_RADIUS
};
void unity_stripe_texture_rounded_outline (UnityStripeTexture* self, cairo_t* cr, gint width, gint height);
static void _unity_stripe_texture_rounded_outline_unity_stripe_texture_stripe_texture_outline_func (cairo_t* cr, gint width, gint height, gpointer self);
UnityStripeTexture* unity_stripe_texture_new (UnityStripeTextureStripeTextureOutlineFunc func, void* func_target);
UnityStripeTexture* unity_stripe_texture_construct (GType object_type, UnityStripeTextureStripeTextureOutlineFunc func, void* func_target);
float unity_stripe_texture_get_radius (UnityStripeTexture* self);
static void unity_stripe_texture_paint_bg (UnityStripeTexture* self, cairo_t* cr, gint width, gint height);
void unity_stripe_texture_set_radius (UnityStripeTexture* self, float value);
static void _unity_stripe_texture_paint_bg_unity_cairo_canvas_cairo_canvas_paint (cairo_t* cr, gint width, gint height, gpointer self);
static GObject * unity_stripe_texture_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static void unity_stripe_texture_finalize (GObject* obj);
static void unity_stripe_texture_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void unity_stripe_texture_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);



static void _unity_stripe_texture_rounded_outline_unity_stripe_texture_stripe_texture_outline_func (cairo_t* cr, gint width, gint height, gpointer self) {
	unity_stripe_texture_rounded_outline (self, cr, width, height);
}


UnityStripeTexture* unity_stripe_texture_construct (GType object_type, UnityStripeTextureStripeTextureOutlineFunc func, void* func_target) {
	UnityStripeTexture * self;
	UnityStripeTextureStripeTextureOutlineFunc _tmp0_;
	self = (UnityStripeTexture*) g_object_new (object_type, "radius", 10.0f, NULL);
	self->outline_paint_func = (_tmp0_ = _unity_stripe_texture_rounded_outline_unity_stripe_texture_stripe_texture_outline_func, ((self->outline_paint_func_target_destroy_notify == NULL) ? NULL : (self->outline_paint_func_target_destroy_notify (self->outline_paint_func_target), NULL), self->outline_paint_func = NULL, self->outline_paint_func_target = NULL, self->outline_paint_func_target_destroy_notify = NULL), self->outline_paint_func_target = g_object_ref (self), self->outline_paint_func_target_destroy_notify = g_object_unref, _tmp0_);
	if (func != NULL) {
		UnityStripeTextureStripeTextureOutlineFunc _tmp1_;
		self->outline_paint_func = (_tmp1_ = func, ((self->outline_paint_func_target_destroy_notify == NULL) ? NULL : (self->outline_paint_func_target_destroy_notify (self->outline_paint_func_target), NULL), self->outline_paint_func = NULL, self->outline_paint_func_target = NULL, self->outline_paint_func_target_destroy_notify = NULL), self->outline_paint_func_target = func_target, self->outline_paint_func_target_destroy_notify = NULL, _tmp1_);
	}
	return self;
}


UnityStripeTexture* unity_stripe_texture_new (UnityStripeTextureStripeTextureOutlineFunc func, void* func_target) {
	return unity_stripe_texture_construct (UNITY_TYPE_STRIPE_TEXTURE, func, func_target);
}


void unity_stripe_texture_rounded_outline (UnityStripeTexture* self, cairo_t* cr, gint width, gint height) {
	gint x;
	gint y;
	g_return_if_fail (self != NULL);
	g_return_if_fail (cr != NULL);
	x = 0;
	y = 0;
	width = width - 1;
	height = height - 1;
	cairo_line_to (cr, (double) x, (double) (y + self->priv->_radius));
	cairo_curve_to (cr, (double) x, (double) y, (double) x, (double) y, (double) (x + self->priv->_radius), (double) y);
	cairo_line_to (cr, (double) (width - self->priv->_radius), (double) y);
	cairo_curve_to (cr, (double) width, (double) y, (double) width, (double) y, (double) width, (double) (y + self->priv->_radius));
	cairo_line_to (cr, (double) width, (double) (height - self->priv->_radius));
	cairo_curve_to (cr, (double) width, (double) height, (double) width, (double) height, (double) (width - self->priv->_radius), (double) height);
	cairo_line_to (cr, (double) (x + self->priv->_radius), (double) height);
	cairo_curve_to (cr, (double) x, (double) height, (double) x, (double) height, (double) x, (double) (height - self->priv->_radius));
	cairo_close_path (cr);
}


static void unity_stripe_texture_paint_bg (UnityStripeTexture* self, cairo_t* cr, gint width, gint height) {
	cairo_pattern_t* pat;
	g_return_if_fail (self != NULL);
	g_return_if_fail (cr != NULL);
	cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
	cairo_set_line_width (cr, 1.0);
	cairo_translate (cr, 0.5, 0.5);
	self->outline_paint_func (cr, width, height, self->outline_paint_func_target);
	if (self->priv->pattern == NULL) {
		cairo_surface_t* _tmp0_;
		cairo_t* context;
		self->priv->pattern = (_tmp0_ = cairo_surface_create_similar (cairo_get_target (cr), CAIRO_CONTENT_COLOR_ALPHA, 4, 4), _cairo_surface_destroy0 (self->priv->pattern), _tmp0_);
		context = cairo_create (self->priv->pattern);
		cairo_set_operator (context, CAIRO_OPERATOR_CLEAR);
		cairo_paint (context);
		cairo_set_line_width (context, 0.3);
		cairo_set_operator (context, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgba (context, 1.0, 1.0, 1.0, 0.85);
		cairo_move_to (context, (double) 0, (double) 0);
		cairo_line_to (context, (double) 4, (double) 4);
		cairo_stroke (context);
		_cairo_destroy0 (context);
	}
	pat = cairo_pattern_create_for_surface (self->priv->pattern);
	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	cairo_set_line_width (cr, 1.75);
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.5);
	cairo_stroke (cr);
	_cairo_pattern_destroy0 (pat);
}


float unity_stripe_texture_get_radius (UnityStripeTexture* self) {
	float result;
	g_return_val_if_fail (self != NULL, 0.0F);
	result = self->priv->_radius;
	return result;
}


void unity_stripe_texture_set_radius (UnityStripeTexture* self, float value) {
	g_return_if_fail (self != NULL);
	self->priv->_radius = value;
	g_object_notify ((GObject *) self, "radius");
}


static void _unity_stripe_texture_paint_bg_unity_cairo_canvas_cairo_canvas_paint (cairo_t* cr, gint width, gint height, gpointer self) {
	unity_stripe_texture_paint_bg (self, cr, width, height);
}


static GObject * unity_stripe_texture_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	GObjectClass * parent_class;
	UnityStripeTexture * self;
	parent_class = G_OBJECT_CLASS (unity_stripe_texture_parent_class);
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = UNITY_STRIPE_TEXTURE (obj);
	{
		UnityCairoCanvasCairoCanvasPaint _tmp0_;
		((UnityCairoCanvas*) self)->paint_func = (_tmp0_ = _unity_stripe_texture_paint_bg_unity_cairo_canvas_cairo_canvas_paint, ((((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target_destroy_notify == NULL) ? NULL : (((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target_destroy_notify (((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target), NULL), ((UnityCairoCanvas*) self)->paint_func = NULL, ((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target = NULL, ((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target_destroy_notify = NULL), ((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target = g_object_ref (self), ((UnityCairoCanvas*) ((UnityCairoCanvas*) self))->paint_func_target_destroy_notify = g_object_unref, _tmp0_);
	}
	return obj;
}


static void unity_stripe_texture_class_init (UnityStripeTextureClass * klass) {
	unity_stripe_texture_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (UnityStripeTexturePrivate));
	G_OBJECT_CLASS (klass)->get_property = unity_stripe_texture_get_property;
	G_OBJECT_CLASS (klass)->set_property = unity_stripe_texture_set_property;
	G_OBJECT_CLASS (klass)->constructor = unity_stripe_texture_constructor;
	G_OBJECT_CLASS (klass)->finalize = unity_stripe_texture_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), UNITY_STRIPE_TEXTURE_RADIUS, g_param_spec_float ("radius", "radius", "radius", -G_MAXFLOAT, G_MAXFLOAT, 0.0F, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
}


static void unity_stripe_texture_instance_init (UnityStripeTexture * self) {
	self->priv = UNITY_STRIPE_TEXTURE_GET_PRIVATE (self);
}


static void unity_stripe_texture_finalize (GObject* obj) {
	UnityStripeTexture * self;
	self = UNITY_STRIPE_TEXTURE (obj);
	(self->outline_paint_func_target_destroy_notify == NULL) ? NULL : (self->outline_paint_func_target_destroy_notify (self->outline_paint_func_target), NULL);
	self->outline_paint_func = NULL;
	self->outline_paint_func_target = NULL;
	self->outline_paint_func_target_destroy_notify = NULL;
	_cairo_surface_destroy0 (self->priv->pattern);
	G_OBJECT_CLASS (unity_stripe_texture_parent_class)->finalize (obj);
}


GType unity_stripe_texture_get_type (void) {
	static volatile gsize unity_stripe_texture_type_id__volatile = 0;
	if (g_once_init_enter (&unity_stripe_texture_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (UnityStripeTextureClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) unity_stripe_texture_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (UnityStripeTexture), 0, (GInstanceInitFunc) unity_stripe_texture_instance_init, NULL };
		GType unity_stripe_texture_type_id;
		unity_stripe_texture_type_id = g_type_register_static (UNITY_TYPE_CAIRO_CANVAS, "UnityStripeTexture", &g_define_type_info, 0);
		g_once_init_leave (&unity_stripe_texture_type_id__volatile, unity_stripe_texture_type_id);
	}
	return unity_stripe_texture_type_id__volatile;
}


static void unity_stripe_texture_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	UnityStripeTexture * self;
	self = UNITY_STRIPE_TEXTURE (object);
	switch (property_id) {
		case UNITY_STRIPE_TEXTURE_RADIUS:
		g_value_set_float (value, unity_stripe_texture_get_radius (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void unity_stripe_texture_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	UnityStripeTexture * self;
	self = UNITY_STRIPE_TEXTURE (object);
	switch (property_id) {
		case UNITY_STRIPE_TEXTURE_RADIUS:
		unity_stripe_texture_set_radius (self, g_value_get_float (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}




