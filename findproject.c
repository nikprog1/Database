#include "main.h"

// Структуры данных для виджета
GtkListStore        *liststore_FindProject; // Буфер таблицы
GtkTreeView         *treeview_FindProject; // Таблица
GtkTreeViewColumn   *column;    // Отображаемая колонка
GtkTreeIter         iter;       // Итератор таблицы (текущая строка)
GtkCellRenderer     *renderer;  // Рендер таблицы (текущая ячейка)
GtkEntry            *gFind; // Поле ввода

int switcher=1;
char Find[100];





G_MODULE_EXPORT void on_radiobutton1_clicked (GtkButton *button, gpointer label)
{
    switcher=1;
}

G_MODULE_EXPORT void on_radiobutton2_clicked (GtkButton *button, gpointer label)
{
    switcher=2;
}

G_MODULE_EXPORT void on_radiobutton_Obj_clicked (GtkButton *button, gpointer label)
{
    switcher=3;
}

G_MODULE_EXPORT void on_radiobutton_rasp_clicked (GtkButton *button, gpointer label)
{
    switcher=4;
}

G_MODULE_EXPORT void on_radiobutton_zakaz_clicked (GtkButton *button, gpointer label)
{
    switcher=5;
}

G_MODULE_EXPORT void on_radiobutton_nom_clicked (GtkButton *button, gpointer label)
{
    switcher=6;
}

G_MODULE_EXPORT void on_radiobutton_man_clicked (GtkButton *button, gpointer label)
{
    switcher=7;
}

G_MODULE_EXPORT void on_radiobutton_proj_clicked (GtkButton *button, gpointer label)
{
    switcher=8;
}

G_MODULE_EXPORT void on_radiobutton_date_clicked (GtkButton *button, gpointer label)
{
    switcher=9;
}

G_MODULE_EXPORT void on_radiobutton_prim_clicked (GtkButton *button, gpointer label)
{
    switcher=10;
}

// Обозначения полей выборки
enum
{
    C_ID_LENGf, C_NAZ_USTf, C_MARK_CABLEf, C_NAME_PLACEf, C_NAZV_SITUf, C_NAZV_ZAKAZf, C_REG_NOMf, C_MANAGERf, C_PROJECTERf, C_DATEf, C_PRIMf
};

int Loadfound()
{
    sprintf(Find, "%s%s%s","'%%",gtk_entry_get_text(gFind),"%%'");
// Очистка таблицы
    gtk_list_store_clear(liststore_FindProject);
// Формирование строки запроса к базе данных
    lenstr = 0;
    lenstr+=sprintf(str, "SELECT [Код длины установленного кабеля],[Название установки],[Марка кабеля],[Название объекта],[Географическое расположение],[Название заказчика],[Регистрационный номер],Менеджер,Проектировщик,Дата,Примечания ");
    lenstr+=sprintf(str+lenstr, " FROM Данные AS a ");
    lenstr+=sprintf(str+lenstr, " JOIN [Установленный кабель] AS c ON a.[Код регистрационного номера] = c.[Код регистрационного номера]");
    lenstr+=sprintf(str+lenstr, " JOIN [Марки кабеля] AS b ON c.[Код марки кабеля] = b.[Код марки кабеля]");

    // Создание последней строки запроса
    switch (switcher)
    {
    case 1:
        lenstr+=sprintf(str+lenstr, " WHERE [Название установки] LIKE %s",Find);
        break;
    case 2:
        lenstr+=sprintf(str+lenstr, " WHERE [Марка кабеля] LIKE %s",Find);
        break;
    case 3:
        lenstr+=sprintf(str+lenstr, " WHERE [Название объекта] LIKE %s",Find);
        break;
    case 4:
        lenstr+=sprintf(str+lenstr, " WHERE [Географическое расположение] LIKE %s",Find);
        break;
    case 5:
        lenstr+=sprintf(str+lenstr, " WHERE [Название заказчика] LIKE %s",Find);
        break;
    case 6:
        lenstr+=sprintf(str+lenstr, " WHERE [Регистрационный номер] LIKE %s",Find);
        break;
    case 7:
        lenstr+=sprintf(str+lenstr, " WHERE [Менеджер] LIKE %s",Find);
        break;
    case 8:
        lenstr+=sprintf(str+lenstr, " WHERE [Проектировщик] LIKE %s",Find);
        break;
    case 9:
        lenstr+=sprintf(str+lenstr, " WHERE [Дата] LIKE %s",Find);
        break;
    case 10:
        lenstr+=sprintf(str+lenstr, " WHERE [Примечания] LIKE %s",Find);
        break;

    }

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
        gtk_list_store_append (liststore_FindProject, &iter);
        gtk_list_store_set (liststore_FindProject, &iter,
                            C_ID_LENGf, sqlite3_column_int(stmt, C_ID_LENGf),
                            C_NAZ_USTf, sqlite3_column_text(stmt, C_NAZ_USTf),
                            C_MARK_CABLEf, sqlite3_column_text(stmt, C_MARK_CABLEf),
                            C_NAME_PLACEf, sqlite3_column_text(stmt,  C_NAME_PLACEf),
                            C_NAZV_SITUf, sqlite3_column_text(stmt, C_NAZV_SITUf),
                            C_NAZV_ZAKAZf, sqlite3_column_text(stmt, C_NAZV_ZAKAZf),
                            C_REG_NOMf, sqlite3_column_text(stmt, C_REG_NOMf),
                            C_MANAGERf, sqlite3_column_text(stmt, C_MANAGERf),
                            C_PROJECTERf, sqlite3_column_text(stmt, C_PROJECTERf),
                            C_DATEf, sqlite3_column_text(stmt, C_DATEf),
                            C_PRIMf, sqlite3_column_text(stmt, C_PRIMf),
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

int Openfound()
{
    int buf;
    int buf1;
// Создание строки и получение модели таблицы виджета
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_FindProject);
// Получение выделенной строки
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_FindProject);

    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;
        // Получение кода установленного кабеля
        gtk_tree_model_get (model, &iter,
                            C_ID_LENGf, &id_data,
                            -1);
        buf=id_data;
    }
    else
    {
        return 0;
    }

// Подготовка запроса к БД
    char* SQL="SELECT [Код регистрационного номера] \
    FROM [Установленный кабель]\
    WHERE [Код длины установленного кабеля]=";
    sprintf(str, "%s%d", SQL, buf);

    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

// Выполнение SQL-запроса
    rc = sqlite3_step(stmt);
// Чтение из БД кода регистрационного номера
    buf1=sqlite3_column_int(stmt, C_ID_LENGf);
// Запуск функции
    ShowChange(buf1);

// Выполнение SQL-запроса и запись в буфер таблицы
    sqlite3_finalize(stmt);
    return 0;
}



// Кнопка поиск
G_MODULE_EXPORT void on_button34_clicked (GtkButton *button, gpointer label)
{
    if(Loadfound())
    {
        ShowError();
    }
}

// Кнопка открыть
G_MODULE_EXPORT void on_button36_clicked (GtkButton *button, gpointer label)
{
Openfound();
   /* // Переключение на существующее окно, если оно открыто
   if (activateChan==1)
    {
        gtk_window_present(GTK_WINDOW(wChange));
    }
    else
    {
        Openfound();
    }*/
}

// Кнопка закрыть
G_MODULE_EXPORT void on_button35_clicked (GtkButton *button, gpointer label)
{
   // activateFind=0;
    gtk_widget_destroy(wFindProject);
}


// Вывод таблицы
void ShowFound()
{
    gchar* text="Запрос";
    builder_FindProject = gtk_builder_new ();
    gtk_builder_add_from_file (builder_FindProject, "main.glade", NULL);
    wFindProject = GTK_WIDGET(gtk_builder_get_object(builder_FindProject, "wFindProject"));
    liststore_FindProject =  GTK_LIST_STORE(gtk_builder_get_object(builder_FindProject, "liststore_FindProject"));
    treeview_FindProject =  GTK_TREE_VIEW(gtk_builder_get_object(builder_FindProject, "treeview_FindProject"));
    gFind =  GTK_ENTRY(gtk_builder_get_object(builder_FindProject, "entry_Find"));
    gtk_entry_set_placeholder_text(gFind,text);
    gtk_window_set_default_size (GTK_WINDOW (wFindProject), 1900, 800);
    gtk_window_set_position(GTK_WINDOW(wFindProject), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wFindProject), "Find");//Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wFindProject), "tables-icon.png", NULL); //Вывод иконки
    //gtk_window_set_transient_for(GTK_WINDOW(wFindProject),GTK_WINDOW(wProjects));
    gtk_window_set_modal(GTK_WINDOW(wFindProject),TRUE);
    gtk_builder_connect_signals (builder_FindProject, NULL);
    gtk_widget_show_all(wFindProject);
}
