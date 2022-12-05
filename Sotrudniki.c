#include "main.h"

// Структуры данных для виджета
GtkListStore        *liststore_Sotr;    // Буфер таблицы
GtkTreeView         *treeview_Sotr; // Таблица
GtkTreeViewColumn   *column;    // Отображаемая колонка
GtkTreeIter         iter;       // Итератор таблицы (текущая строка)



int StatusToggled=1;
// Обозначения полей таблицы "Сотрудники"
enum
{
    C_ID_SOTR,C_FAMIL_SOTR,C_NAME_SOTR,C_DOLJN_SOTR, C_KOD_SOTRUDNICHESTVA_SOTR
};

// Загрузка таблицы из БД в буфер виджета
int LoadSotr()
{
    gtk_list_store_clear (liststore_Sotr);
    char* SQL = "SELECT [Код сотрудника],\
       Фамилия,Имя,Должность,Сотрудничество FROM Сотрудники\
       WHERE Сотрудничество=1";

    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Цикл по SQL-запросу и запись в буфер таблицы
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        gtk_list_store_append (liststore_Sotr, &iter);
        gtk_list_store_set (liststore_Sotr, &iter,
                            C_ID_SOTR, sqlite3_column_int(stmt, C_ID_SOTR),
                            C_FAMIL_SOTR, sqlite3_column_text(stmt, C_FAMIL_SOTR),
                            C_NAME_SOTR, sqlite3_column_text(stmt, C_NAME_SOTR),
                            C_DOLJN_SOTR, sqlite3_column_text(stmt, C_DOLJN_SOTR),
                            C_KOD_SOTRUDNICHESTVA_SOTR, sqlite3_column_int(stmt,  C_KOD_SOTRUDNICHESTVA_SOTR),
                            -1);
    }
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 0;
    }

    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}


int AddItemSotr ()
{
// Вставка новой записи в базу
    char* SQL="INSERT INTO Сотрудники (Фамилия,Имя,Должность,Сотрудничество)VALUES('Не указано','Не указано','Не указано',1)";
    sprintf(str,"%s",SQL);
    //ShowSQL();
    // Записываем в БД
    if (sqlite3_exec(db, str, 0, 0, &err))
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
        ShowError();
        sqlite3_free(err);
        return 1;
    }
    // Получение последней вставленной строки
    char* SQL2 = "SELECT [Код сотрудника],Фамилия,Имя,Должность,Сотрудничество FROM Сотрудники\
                ORDER BY [Код сотрудника] DESC LIMIT 1";

    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL2, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);

    gtk_list_store_append (liststore_Sotr, &iter);
    gtk_list_store_set (liststore_Sotr, &iter,
                            C_ID_SOTR, sqlite3_column_int(stmt, C_ID_SOTR),
                            C_FAMIL_SOTR, sqlite3_column_text(stmt, C_FAMIL_SOTR),
                            C_NAME_SOTR, sqlite3_column_text(stmt, C_NAME_SOTR),
                            C_DOLJN_SOTR, sqlite3_column_text(stmt, C_DOLJN_SOTR),
                            C_KOD_SOTRUDNICHESTVA_SOTR, sqlite3_column_int(stmt,  C_KOD_SOTRUDNICHESTVA_SOTR),
                            -1);

    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}



// Удаление записей из БД
int DeleteItemSotr ()
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_Sotr);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_Sotr);

    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;

        // Получение сотрудика
        gtk_tree_model_get (model, &iter,
                            C_ID_SOTR, &id_data,
                            -1);

        // Удаление строки из буфера
        gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

        // Удаление записи из БД таблица данные
        lenstr = 0;
        lenstr+=sprintf(str, "DELETE FROM Сотрудники WHERE [Код сотрудника]=");
        lenstr+=sprintf(str+lenstr, "%d", id_data);
        lenstr+=sprintf(str+lenstr, ";");
        //ShowSQL(); // Тестовый вывод запроса
        // Записываем в БД
        if (sqlite3_exec(db, str, 0, 0, &err))
        {
            sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
			ShowError();
            sqlite3_free(err);
            return 1;
        }

    }
    return 0;
}
// Скрытие и открытие бывших и текущих сотрудников
int StatusSotr(int status)
{
    gtk_list_store_clear (liststore_Sotr);
    char* SQL = "SELECT [Код сотрудника],\
       Фамилия,Имя,Должность,Сотрудничество FROM Сотрудники\
       WHERE Сотрудничество=";
    sprintf(str,"%s%d",SQL,status);
    //ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Цикл по SQL-запросу и запись в буфер таблицы
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        gtk_list_store_append (liststore_Sotr, &iter);
        gtk_list_store_set (liststore_Sotr, &iter,
                            C_ID_SOTR, sqlite3_column_int(stmt, C_ID_SOTR),
                            C_FAMIL_SOTR, sqlite3_column_text(stmt, C_FAMIL_SOTR),
                            C_NAME_SOTR, sqlite3_column_text(stmt, C_NAME_SOTR),
                            C_DOLJN_SOTR, sqlite3_column_text(stmt, C_DOLJN_SOTR),
                            C_KOD_SOTRUDNICHESTVA_SOTR, sqlite3_column_int(stmt,  C_KOD_SOTRUDNICHESTVA_SOTR),
                            -1);
    }
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 0;
    }

    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// Обновить запись таблицы данные
int ReloadSotr(int sotr) // Обновить запись таблицы данные
{
    GtkTreeIter         iter;
/*  GtkTreeModel *model = gtk_tree_view_get_model (treeview_Sotr); //Получаем модель виджета
    gtk_tree_model_get_iter (model, &iter, path); //Получаем итератор из модели по пути path*/
    // Получение записи
    char* SQL = "SELECT [Код сотрудника],Фамилия,Имя,Должность,Сотрудничество\
  FROM Сотрудники WHERE [Код сотрудника] =";

    // Готовим SQL-запрос к БД
    lenstr = 0;
    lenstr+=sprintf(str, SQL);
    lenstr+=sprintf(str+lenstr, "%d", sotr);
    ShowSQL(); // Тестовый вывод запроса
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);

    gtk_list_store_set (liststore_Sotr, &iter,
                            C_ID_SOTR, sqlite3_column_int(stmt, C_ID_SOTR),
                            C_FAMIL_SOTR, sqlite3_column_text(stmt, C_FAMIL_SOTR),
                            C_NAME_SOTR, sqlite3_column_text(stmt, C_NAME_SOTR),
                            C_DOLJN_SOTR, sqlite3_column_text(stmt, C_DOLJN_SOTR),
                            C_KOD_SOTRUDNICHESTVA_SOTR, sqlite3_column_int(stmt,  C_KOD_SOTRUDNICHESTVA_SOTR),
                            -1); //Заполняем строку таблицы



    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

/* //Отслеживание открытия окна "Данные сотрудника"
G_MODULE_EXPORT void on_wChangeSotr_show (GtkButton *button, gpointer label)

{
    activateChanSotr=1;
}

//Отслеживание закрытия окна "Данные сотрудника"
G_MODULE_EXPORT void on_wChangeSotr_destroy (GtkButton *button, gpointer label)

{
    activateChanSotr=0;
}
 */

// Кнопка Добавить
G_MODULE_EXPORT void on_button5_clicked (GtkButton *button, gpointer label)
{
    AddItemSotr ();
}

// Кнопка Удалить
G_MODULE_EXPORT void on_button8_clicked (GtkButton *button, gpointer label)
{
	if (ShowQuestion())
    {
		DeleteItemSotr ();
    }
}

// Кнопка скрыть
G_MODULE_EXPORT void on_togglebutton1_toggled (GtkButton *button, gpointer label)
{
    if (StatusToggled==1)
    {
    StatusSotr(StatusToggled=0);
    }
    else
    {
    StatusSotr(StatusToggled=1);
    }
}


// Кнопка Открыть
G_MODULE_EXPORT void on_button4_clicked (GtkButton *button, gpointer label)
{

	// Определяем текущий код
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_Sotr);//Получаем модель виджета
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_Sotr); //Получаем выделенную строку
    gtk_tree_selection_get_selected(selection, &tm, &iter);//Получаем итератор и модель выделенного
    path = gtk_tree_model_get_path ( tm , &iter ); //Получаем путь к итератору
    /*int *i;
    i = gtk_tree_path_get_indices ( path ) ;
    memSel = i [ 0 ] ;*/
    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;
        // Получение кода регистрационного номера
        gtk_tree_model_get (model, &iter,
                            C_ID_SOTR, &id_data,
                            -1);
        ShowChangeSotr(id_data,StatusToggled);
    }

     /* if (activateChanSotr==1)
    {
        gtk_window_present(GTK_WINDOW(wChangeSotr));
    }
    else
    {
// Определяем текущий код
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_Sotr);//Получаем модель виджета
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_Sotr); //Получаем выделенную строку
    gtk_tree_selection_get_selected(selection, &tm, &iter);//Получаем итератор и модель выделенного
    path = gtk_tree_model_get_path ( tm , &iter ); //Получаем путь к итератору
    //int *i;
    i = gtk_tree_path_get_indices ( path ) ;
    memSel = i [ 0 ] ;
    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;
        // Получение кода регистрационного номера
        gtk_tree_model_get (model, &iter,
                            C_ID_SOTR, &id_data,
                            -1);
        ShowChangeSotr(id_data);
    }
    } */
}


// Кнопка Закрыть
G_MODULE_EXPORT void on_button6_clicked (GtkButton *button, gpointer label)
{
    gtk_widget_destroy (wSotr);
}



// Вывод таблицы
void ShowSotr()
{
    bSotr = gtk_builder_new ();
    gtk_builder_add_from_file (bSotr, "main.glade", NULL);
    wSotr = GTK_WIDGET(gtk_builder_get_object(bSotr, "wSotr"));
    liststore_Sotr =  GTK_LIST_STORE(gtk_builder_get_object(bSotr, "liststore_Sotr")); // Хранилище строк
    treeview_Sotr =  GTK_TREE_VIEW(gtk_builder_get_object(bSotr, "treeview_Sotr"));
    gtk_window_set_default_size (GTK_WINDOW (wSotr), 1000, 1000);
    gtk_window_set_position(GTK_WINDOW(wSotr), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wSotr), "Sotrudniki");// Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wSotr), "tables-icon.png", NULL); //Вывод иконки
    gtk_window_set_modal(GTK_WINDOW(wSotr),TRUE);
    gtk_builder_connect_signals (bSotr, NULL);
    if(LoadSotr())
    {
        ShowError();
    }
    gtk_widget_show_all(wSotr);
}
