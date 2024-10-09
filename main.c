#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <cmark.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Function to read and convert markdown to HTML
void load_markdown(WebKitWebView *webView, const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		g_print("Failed to open file: %s\n", filename);
		return;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	gchar *markdown = (gchar *)malloc(length + 1);
	if (!markdown) {
		fclose(file);
		g_print("Memory allocation failed\n");
		return;
	}

	fread(markdown, 1, length, file);
	fclose(file);
	markdown[length] = '\0';

	// Convert Markdown to HTML
	cmark_node *node = cmark_parse_document(markdown, length, CMARK_OPT_DEFAULT);
	gchar *html = cmark_render_html(node, CMARK_OPT_DEFAULT);
	cmark_node_free(node);
	free(markdown);

	webkit_web_view_load_html(webView, html, NULL);
	g_free(html);
}

// Callback function for file changes
void file_changed(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data) {
	if (event_type == G_FILE_MONITOR_EVENT_CHANGED) {
		WebKitWebView *webView = WEBKIT_WEB_VIEW(user_data);
		char *filename = g_file_get_path(file);
		load_markdown(webView, filename);
		g_free(filename);
	}
}

void disable_context_menu(WebKitWebView *webView) {
	g_signal_connect(webView, "context-menu", G_CALLBACK(gtk_true), NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		g_print("Usage: %s <markdown_file>\n", argv[0]);
		g_print("This program renders a Markdown file in a GUI window.\n");
		return 1;
	}

	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

	WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));

	load_markdown(webView, argv[1]);

	disable_context_menu(webView);  // Disable context menu

	// Monitor file for changes
	GFile *file = g_file_new_for_path(argv[1]);
	GFileMonitor *monitor = g_file_monitor_file(file, G_FILE_MONITOR_NONE, NULL, NULL);
	g_signal_connect(monitor, "changed", G_CALLBACK(file_changed), webView);
	g_object_unref(file);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
