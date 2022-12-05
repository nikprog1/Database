/*********************************************************************
Таблица проектов
**********************************************************************/
#include "main.h"
// Открываем БД
void StartDB()
{
    if( sqlite3_open("ProjectsPO.db", &db))
    {
        sprintf(str, "Ошибка открытия БД: %s\n", sqlite3_errmsg(db));
		ShowError();
    }
}

// Закрываем БД
void EndDB()
{
    sqlite3_close(db);
}

void ShowError()
{
    GtkWidget *dialog = NULL;

    dialog = gtk_message_dialog_new (GTK_WINDOW (wMain), GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, str);
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

int ShowSQL()
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                    str);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return 0;
}


int ShowGladeError()
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                 "Не найден файл\n main.glade");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return 0;
}


int ShowQuestion()
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                 "Удалить безвозратно?");

    if (gtk_dialog_run(GTK_DIALOG(dialog))==-8)
    {
    gtk_widget_destroy(dialog);
    return 1;
    }
    else
    {
    gtk_widget_destroy(dialog);
    return 0;
    }
}

int ShowQuestionAdd()
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                 "Добавить выделенную строку?");

    if (gtk_dialog_run(GTK_DIALOG(dialog))==-8)
    {
    gtk_widget_destroy(dialog);
    return 1;
    }
    else
    {
    gtk_widget_destroy(dialog);
    return 0;
    }
}





//О
int ShowInfo()
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,"Введенный пароль не подходит");
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    if (gtk_dialog_run(GTK_DIALOG(dialog))==-8);
    {
        gtk_widget_destroy(dialog);
    }
    return 0;
}




/* //Отслеживание открытия окна "Проекты"
G_MODULE_EXPORT void on_wProjects_show (GtkButton *button, gpointer label)

{
    activateproj=1;
} */

/* //Отслеживание закрытия окна "Проекты"
G_MODULE_EXPORT void on_wProjects_destroy (GtkButton *button, gpointer label)

{
    activateproj=0;
} */


// Кнопка проекты
G_MODULE_EXPORT void on_button16_clicked (GtkButton *button, gpointer label)
{
ShowProjects();
}


// Кнопка Выход
G_MODULE_EXPORT void on_button3_clicked (GtkButton *button, gpointer label)
{
    gtk_widget_destroy (wMain);
}

/*G_MODULE_EXPORT void on_button23_clicked (GtkButton *button, gpointer label)
{
    ShowSettings();
}
*/

// Кнопка Поиск
G_MODULE_EXPORT void on_button10_clicked (GtkButton *button, gpointer label)
{
    ShowFound();
    /*if (activateFind==1)
    {
    gtk_window_present(GTK_WINDOW(wFindProject));
    }
   else
   {
       ShowFound();
   }*/
}


// Кнопка Сотрудники
G_MODULE_EXPORT void on_button7_clicked (GtkButton *button, gpointer label)
{
    ShowSotr();
}



int ShowMainWindow()
{
	builder1 = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder1, "main.glade", NULL))
	{
        wMain = GTK_WIDGET(gtk_builder_get_object(builder1, "wMain"));

        gtk_window_set_default_size (GTK_WINDOW (wMain), 500, 300);
        gtk_window_set_position(GTK_WINDOW(wMain), GTK_WIN_POS_CENTER);
        gtk_window_set_title (GTK_WINDOW (wMain), "Main menu");// Название окна
        gtk_window_set_icon_from_file (GTK_WINDOW(wMain), "tables-icon.png", NULL); //Вывод иконки
	    gtk_builder_connect_signals (builder1, NULL);
        g_signal_connect(G_OBJECT(wMain), "destroy", G_CALLBACK(gtk_main_quit), NULL);
        gtk_widget_show (wMain);
        return TRUE;
	}
    else
	{
	    return FALSE;
	}
}

int main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    StartDB();
    if (ShowMainWindow())
    {
        gtk_main ();
        EndDB();
        return 0;
    }
    else
    {
       ShowGladeError();
       EndDB();
       return 1;
    }
}
