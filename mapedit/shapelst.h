/**
 **	A GTK widget showing a list of shapes from an image file.
 **
 **	Written: 7/25/99 - JSF
 **/

#ifndef INCL_SHAPELST
#define INCL_SHAPELST	1

/*
Copyright (C) 1999  Jeffrey S. Freedman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "objbrowse.h"
#include "shapedraw.h"
#include "rect.h"

class Vga_file;
class Image_buffer8;
class Shapes_vga_file;
class Shape_group;

/*
 *	Store information about an individual shape shown in the list.
 */
class Shape_entry
	{
	friend class Shape_chooser;
	int shapenum, framenum;		// The given shape/frame.
	Rectangle box;			// Box where drawn.
	Shape_entry() {  }
	void set(int shnum, int frnum, int rx, int ry, int rw, int rh)
		{
		shapenum = shnum; framenum = frnum;
		box = Rectangle(rx, ry, rw, rh);
		}
	};

/*
 *	This class manages a list of shapes from an image file.
 */
class Shape_chooser: public Object_browser, public Shape_draw
	{
	Shapes_vga_file *shapes_file;	// Non-null if 'shapes.vga'.
	Shape_group *group;		// Non-null to use filter.
	GtkWidget *sbar;		// Status bar.
	guint sbar_sel;			// Status bar context for selection.
	GtkWidget *fspin;		// Spin button for frame #.
	GtkWidget *shape_scroll;	// Vertical scrollbar.
	GtkWidget *find_text;		// For searching.
	GtkAdjustment *frame_adj;	// Adjustment for frame spin btn.
	int shapenum0;			// Shape, frame # of leftmost in
					//   displayed list.
	int framenum0;			// Default frame # to display.
	Shape_entry *info;		// An entry for each shape drawn.
	int info_cnt;			// # entries in info.
	int num_per_row;		// Average # painted per line.
	int selected;			// Index of user-selected entry.
	void (*sel_changed)();		// Called when selection changes.
					// Blit onto screen.
	void show(int x, int y, int w, int h);
	void show()
		{ show(0, 0, draw->allocation.width, draw->allocation.height);}
	void tell_server_shape();	// Tell Exult what shape is selected.
	void select(int new_sel);	// Show new selection.
	virtual void render();		// Draw list.
	void scroll(int newindex);	// Scroll.
	void adjust_scrollbar();	// Set new scroll amounts.
	GtkWidget *create_search_controls();
public:
	Shape_chooser(Vga_file *i, unsigned char *palbuf, int w, int h);
	virtual ~Shape_chooser();
	void set_shapes_file(Shapes_vga_file *sh)
		{ shapes_file = sh; }	
	void set_group(Shape_group *g)
		{ group = g; }
	void set_framenum0(int f)
		{ framenum0 = f; }
	void search(char *srch, int dir);
					// Turn off selection.
	void unselect(bool need_render = true);
	int is_selected()		// Is a shape selected?
		{ return selected >= 0; }
	void set_selected_callback(void (*fun)())
		{ sel_changed = fun; }
					// Get selected shape, or return 0.
	int get_selected(int& shapenum, int& framenum)
		{
		if (selected == -1)
			return (0);
		shapenum = info[selected].shapenum;
		framenum = info[selected].framenum;
		return (1);
		}
	GtkWidget *get_find_text()	// Get 'find' text widget.
		{ return find_text; }
					// Configure when created/resized.
	static gint configure(GtkWidget *widget, GdkEventConfigure *event,
							gpointer data);
					// Blit to screen.
	static gint expose(GtkWidget *widget, GdkEventExpose *event,
							gpointer data);
					// Handle mouse press.
	static gint mouse_press(GtkWidget *widget, GdkEventButton *event,
							gpointer data);
	void edit_shape();		// Edit selected shape.
					// Give dragged shape.
	static void drag_data_get(GtkWidget *widget, GdkDragContext *context,
		GtkSelectionData *data, guint info, guint time, gpointer data);
					// Someone else selected.
	static gint selection_clear(GtkWidget *widget,
				GdkEventSelection *event, gpointer data);
	static gint drag_begin(GtkWidget *widget, GdkDragContext *context,
							gpointer data);
					// Handle scrollbar.
	static void scrolled(GtkAdjustment *adj, gpointer data);
					// Handle spin-button for frames.
	static void frame_changed(GtkAdjustment *adj, gpointer data);
#ifdef WIN32
	static gint win32_drag_motion(GtkWidget *widget, GdkEventMotion *event,
		gpointer data);
#endif
	};

#endif
