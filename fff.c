#include "main.h"
enum
{
  DOUBLE_COLUMN,
  N_COLUMNS
};

GtkListStore *mycolumns;
GtkTreeView *treeview;
GtkCellRenderer     *renderer;  // Рендер таблицы (текущая ячейка)

void // Выполняемая функция
my_cell_double_to_text (GtkTreeViewColumn *tree_column,
	                GtkCellRenderer   *cell,
                        GtkTreeModel      *tree_model,
	                GtkTreeIter       *iter,
                        gpointer           data)
{
  GtkCellRendererText *cell_text = (GtkCellRendererText *)cell;
  gdouble d;
  gchar *text;

  /* Get the double value from the model. */
  gtk_tree_model_get (tree_model, iter, (gint)data, &d, -1);
  /* Now we can format the value ourselves. */
  text = g_strdup_printf ("%.2f", d);
  g_object_set (cell, "text", text, NULL);
  g_free (text);
}

void
set_up_new_columns (GtkTreeView *myview)
{
  GtkCellRendererText *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *mycolumns;

  /* Create the data model and associate it with the given TreeView */
  mycolumns = gtk_list_store_new (N_COLUMNS, G_TYPE_DOUBLE);
  gtk_tree_view_set_model (myview, GTK_TREE_MODEL (mycolumns));

  /* Create a GtkCellRendererText */
  renderer = gtk_cell_renderer_text_new ();

  /* Create a new column that has a title ("Example column"),
   * uses the above created renderer that will render the double
   * value into text from the associated model's rows.
   */
  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_title  (column, "Example column");
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (column, renderer, TRUE);

  /* Append the new column after the GtkTreeView's previous columns. */
  gtk_tree_view_append_column (GTK_TREE_VIEW (myview), column);
  /* Since we created the column by hand, we can set it up for our
   * needs, e.g. set its minimum and maximum width, etc.
   */
  /* Set up a custom function that will be called when the column content
   * is rendered. We use the func_data pointer as an index into our
   * model. This is convenient when using multi column lists.
   */
  gtk_tree_view_column_set_cell_data_func (column, renderer,
	                                   my_cell_double_to_text,
                                           (gpointer)DOUBLE_COLUMN, NULL);
}
