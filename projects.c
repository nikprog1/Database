#include "main.h"
#include "time.h"

// Структуры данных для виджета
GtkListStore        *liststore_projects;    // Буфер таблицы
GtkTreeView         *treeview_projects; // Таблица
GtkWidget              *scrolledWindow;
GtkTreeViewColumn   *column;    // Отображаемая колонка
GtkTreeIter         iter;       // Итератор таблицы (текущая строка)
GtkCellRenderer     *renderer;  // Рендер таблицы (текущая ячейка)
GtkCellLayout *cell_layout;
int memSel=-1; // Хранение номера строки в таблице
// Обозначения полей
enum
{
    C_ID, C_REG_NUM, C_OBJ_NAME, C_MESTO_RASP, C_ZAKAZ, C_MANAG, C_PROJECTER, C_DATE, C_PRIM
};


// Загрузка таблицы из БД в буфер виджета
int LoadProjects()
{
    char* SQL = "SELECT [Код регистрационного номера],[Регистрационный номер],[Название объекта],[Географическое расположение],\
                [Название заказчика], Менеджер, Проектировщик, Дата, Примечания\
				FROM Данные";

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
        gtk_list_store_append (liststore_projects, &iter);
        gtk_list_store_set (liststore_projects, &iter,
                            C_ID, sqlite3_column_int(stmt, C_ID),
                            C_REG_NUM, sqlite3_column_text(stmt, C_REG_NUM),
                            C_OBJ_NAME, sqlite3_column_text(stmt, C_OBJ_NAME),
                            C_MESTO_RASP, sqlite3_column_text(stmt,  C_MESTO_RASP),
                            C_ZAKAZ, sqlite3_column_text(stmt, C_ZAKAZ),
                            C_MANAG, sqlite3_column_text(stmt, C_MANAG),
                            C_PROJECTER, sqlite3_column_text(stmt, C_PROJECTER),
                            C_DATE, sqlite3_column_text(stmt, C_DATE),
                            C_PRIM, sqlite3_column_text(stmt, C_PRIM),
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


// Добавить запись
int AddItem ()
{
    char buf[100];
    char buf2[100];
    char regnom[1000];
    // Получение последней вставленной строки
    char* SQL3 = "SELECT [Код регистрационного номера],[Регистрационный номер]\
				FROM Данные\
                ORDER BY [Код регистрационного номера] DESC LIMIT 1";

    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL3, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);
    //Получение из базы данных регистрационного номера проекта
    sprintf(buf,"%s", sqlite3_column_text(stmt, C_REG_NUM));
    //Освобождаем строку запроса
    sqlite3_finalize(stmt);

    lenstr = 0;
    //Получение из базы данных типа системы
    memcpy( buf2, &buf, 4 );
    buf2[4] = '\0';
    lenstr+=sprintf(regnom, "'%s-ПО-", buf2);
    //Получение из базы данных порядкового номера системы и прибавление к нему единицы
    memcpy( buf2, &buf[10], 5 );
    buf2[5] = '\0';
    int n=atoi(buf2);
    n++;
    if (n>=100000)
    {
        return 0;
    }
    lenstr+=sprintf(regnom+lenstr, "%d-", n);
    //Получение из базы данных номера системы в проекте бумажном
    memcpy( buf2, &buf[16], 2 );
    buf2[2] = '\0';
    n=0;
    lenstr+=sprintf(regnom+lenstr, "%d'", n);

    //Извлечение текущей даты
    time_t nowtime;                                     // переменная для хранения времени
    struct tm * timeinfo;                               // структура для хранения временных данных
    char time_to_ins [80];                              // строка, в которой будет храниться текущее время
    time ( &nowtime );                                  // текущая дата в секундах , функция time
    timeinfo = localtime ( &nowtime );                  // текущее локальное время, представленное в структуре
    strftime (time_to_ins,80,"'%Y-%m-%d'",timeinfo);    // форматируем строку времени и записываем в буффер

    GtkTreeIter iter;

    // Вставка новой записи в базу
    char* SQL="INSERT INTO Данные ([Регистрационный номер], Дата) VALUES (";
    sprintf(str,"%s%s,%s)",SQL,regnom,time_to_ins);
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
    char* SQL2 = "SELECT [Код регистрационного номера],[Регистрационный номер],[Название объекта],[Географическое расположение],\
                [Название заказчика], Менеджер, Проектировщик, Дата, Примечания \
				FROM Данные\
                ORDER BY [Код регистрационного номера] DESC LIMIT 1";

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

    gtk_list_store_append (liststore_projects, &iter);
    gtk_list_store_set (liststore_projects, &iter,
                        C_ID, sqlite3_column_int(stmt, C_ID),
                        C_REG_NUM, sqlite3_column_text(stmt, C_REG_NUM),
                        C_OBJ_NAME, sqlite3_column_text(stmt, C_OBJ_NAME),
                        C_MESTO_RASP, sqlite3_column_text(stmt,  C_MESTO_RASP),
                        C_ZAKAZ, sqlite3_column_text(stmt, C_ZAKAZ),
                        C_MANAG, sqlite3_column_text(stmt, C_MANAG),
                        C_PROJECTER, sqlite3_column_text(stmt, C_PROJECTER),
                        C_DATE, sqlite3_column_text(stmt, C_DATE),
                        C_PRIM, sqlite3_column_text(stmt, C_PRIM),
                        -1);

    //создаем модель и выделение
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects);
    //находим путь по итеру
    path = gtk_tree_model_get_path ( model, &iter );
    //убираем выделение
    gtk_tree_selection_unselect_all(selection);
    //выбираем строку по пути и прокручиваем окно к ней
    gtk_tree_selection_select_path(selection, path);
    gtk_tree_view_scroll_to_cell(treeview_projects, path, NULL, FALSE, 1.0, 0.0);
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}


// Продублировать запись
int CopyPastItem ()
{
    char buf[100];
    char buf2[100];
    char regnom[1000];
    // Получение последней вставленной строки
    char* SQL3 = "SELECT [Код регистрационного номера],[Регистрационный номер]\
				FROM Данные\
                ORDER BY [Код регистрационного номера] DESC LIMIT 1";

    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL3, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);
    //Получение из базы данных регистрационного номера проекта
    sprintf(buf,"%s", sqlite3_column_text(stmt, C_REG_NUM));
    //Освобождаем строку запроса
    sqlite3_finalize(stmt);

    lenstr = 0;
    //Получение из базы данных типа системы
    memcpy( buf2, &buf, 4 );
    buf2[4] = '\0';
    lenstr+=sprintf(regnom, "'%s-ПО-", buf2);
    //Получение из базы данных порядкового номера системы и прибавление к нему единицы
    memcpy( buf2, &buf[10], 5 );
    buf2[5] = '\0';
    int n=atoi(buf2);
    n++;
    if (n>=100000)
    {
        return 0;
    }
    lenstr+=sprintf(regnom+lenstr, "%d-", n);
    //Получение из базы данных номера системы в проекте бумажном

    memcpy( buf2, &buf[16], 2 );
    buf2[2] = '\0';
    n=0;
    lenstr+=sprintf(regnom+lenstr, "%d'", n);

    //Извлечение текущей даты
    time_t nowtime;                                     // переменная для хранения времени
    struct tm * timeinfo;                               // структура для хранения временных данных
    char time_to_ins [80];                              // строка, в которой будет храниться текущее время
    time ( &nowtime );                                  // текущая дата в секундах , функция time
    timeinfo = localtime ( &nowtime );                  // текущее локальное время, представленное в структуре
    strftime (time_to_ins,80,"'%Y-%m-%d'",timeinfo);    // форматируем строку времени и записываем в буффер

//Cоздаем модель и выделение
    GtkTreeIter iter;
    gchar *object1,*mesto1,*zakaz1;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects);
    gtk_tree_selection_get_selected(selection, &model, &iter); //Получаем итератор и модель выделенного
    gtk_tree_model_get(model, &iter,2,&object1,3,&mesto1,4,&zakaz1,-1);//Получаем содержимое ячеек

// Вставка новой записи в базу
    char* SQL="INSERT INTO Данные ([Регистрационный номер],[Название объекта],[Географическое расположение],[Название заказчика], Дата) VALUES (";
    sprintf(str,"%s%s,'%s','%s','%s',%s)",SQL,regnom,object1,mesto1,zakaz1,time_to_ins);
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
    char* SQL2 = "SELECT [Код регистрационного номера],[Регистрационный номер],[Название объекта],[Географическое расположение],\
                [Название заказчика], Менеджер, Проектировщик, Дата, Примечания \
				FROM Данные\
                ORDER BY [Код регистрационного номера] DESC LIMIT 1";

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

    gtk_list_store_append (liststore_projects, &iter);
    gtk_list_store_set (liststore_projects, &iter,
                        C_ID, sqlite3_column_int(stmt, C_ID),
                        C_REG_NUM, sqlite3_column_text(stmt, C_REG_NUM),
                        C_OBJ_NAME, sqlite3_column_text(stmt, C_OBJ_NAME),
                        C_MESTO_RASP, sqlite3_column_text(stmt,  C_MESTO_RASP),
                        C_ZAKAZ, sqlite3_column_text(stmt, C_ZAKAZ),
                        C_MANAG, sqlite3_column_text(stmt, C_MANAG),
                        C_PROJECTER, sqlite3_column_text(stmt, C_PROJECTER),
                        C_DATE, sqlite3_column_text(stmt, C_DATE),
                        C_PRIM, sqlite3_column_text(stmt, C_PRIM),
                        -1);

    //находим путь по итеру
    path = gtk_tree_model_get_path ( model, &iter );
    //убираем выделение
    gtk_tree_selection_unselect_all(selection);
    //выбираем строку по пути и прокручиваем окно к ней
    gtk_tree_selection_select_path(selection, path);
    gtk_tree_view_scroll_to_cell(treeview_projects, path, NULL, FALSE, 1.0, 0.0);
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// Удаление записей из БД
int DeleteItem ()
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects);

    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;

        // Получение кода проекта
        gtk_tree_model_get (model, &iter,
                            C_ID, &id_data,
                            -1);
        if (id_data==1)
        {
            return 0;
        }

        // Удаление строки из буфера
        gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

        // Удаление записи из БД таблица данные
        lenstr = 0;
        lenstr+=sprintf(str, "DELETE FROM Данные WHERE [Код регистрационного номера]=");
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

        // Удаление записи из БД таблица [Установленный кабель]
        lenstr = 0;
        lenstr+=sprintf(str, "DELETE FROM [Установленный кабель] WHERE [Код регистрационного номера]=");
        lenstr+=sprintf(str+lenstr, "%d", id_data);
        lenstr+=sprintf(str+lenstr, ";");
        //ShowSQL(); // Тестовый вывод запроса
        // Записываем в БД
        if (sqlite3_exec(db, str, 0, 0, &err))
        {
            sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
            ShowError();
            sqlite3_free(err);
            return 2;
        }
    }
    return 0;
}

// Обновить запись таблицы данные
int ReloadProjects(int proj)
{
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects); //Получаем модель виджета
    gtk_tree_model_get_iter (model, &iter, path); //Получаем итератор из модели по пути path
    // Получение записи
    char* SQL = "SELECT\
    [Код регистрационного номера],\
    [Регистрационный номер],\
    [Название объекта],\
    [Географическое расположение],\
    [Название заказчика],\
    Менеджер,\
    Проектировщик,\
    Дата,\
    Примечания\
    FROM Данные\
    WHERE [Код регистрационного номера]=";

    // Готовим SQL-запрос к БД
    lenstr = 0;
    lenstr+=sprintf(str, SQL);
    lenstr+=sprintf(str+lenstr, "%d", proj);
    //ShowSQL(); // Тестовый вывод запроса
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);

    gtk_list_store_set (liststore_projects, &iter,
                        C_ID, sqlite3_column_int(stmt, C_ID),
                        C_REG_NUM, sqlite3_column_text(stmt, C_REG_NUM),
                        C_OBJ_NAME, sqlite3_column_text(stmt, C_OBJ_NAME),
                        C_MESTO_RASP, sqlite3_column_text(stmt,  C_MESTO_RASP),
                        C_ZAKAZ, sqlite3_column_text(stmt, C_ZAKAZ),
                        C_MANAG, sqlite3_column_text(stmt, C_MANAG),
                        C_PROJECTER, sqlite3_column_text(stmt, C_PROJECTER),
                        C_DATE, sqlite3_column_text(stmt, C_DATE),
                        C_PRIM, sqlite3_column_text(stmt, C_PRIM),
                        -1); //Заполняем строку таблицы



    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

void // Выполняемая функция
my_cell (GtkTreeViewColumn *column, GtkCellRenderer *renderer,GtkTreeModel *model, GtkTreeIter *iter, gpointer label)
{
    gfloat xalign,yalign;
    xalign=4.1;
    yalign=4.1;
    model = gtk_tree_view_get_model (treeview_projects); //Получаем модель виджета
   gtk_tree_model_get_iter_first (model, iter);//Получаем итер на первую стоку
    //gtk_tree_model_set (model, &iter,-1);
    //gtk_cell_renderer_set_alignment (renderer,xalign, yalign);
    g_object_set (renderer, "background", "red", FALSE, NULL);
}


void find_for_past ()
{
int N_column=2, N_row=0; //Координаты ячейки
gchar *object2,*mesto2,*zakaz2;//Содержимое ячеек
int n_symbols;//Число символов в запросе
char zapros[1000];//Содержимое запроса
//gboolean test;
//int testint;
char test[100];
GtkTreeIter iter;
GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects); //Получаем модель виджета
gtk_tree_model_get_iter_first (model, &iter);//Получаем итер на первую стоку
gtk_tree_model_get(model, &iter,2,&object2,-1);//Получаем содержимое ячеек

//sprintf(test,"%s",object2);
//sprintf(zapros,"%s","1");
n_symbols=strlen(object2); //Получаем количество символов в ячейке
strnicmp(object2,zapros,n_symbols); //Сравниваем n-число символов c запросом
//gtk_tree_view_column_set_cell_data_func(column,renderer,my_cell,NULL,NULL);
g_object_set (renderer, "background", "yellow", FALSE, NULL);

}

/* //Отслеживание открытия окна "Данные проекта"
G_MODULE_EXPORT void on_wChange_show (GtkButton *button, gpointer label)

{
    activateChan=1;
} */

/* //Отслеживание закрытия окна "Данные проекта"
G_MODULE_EXPORT void on_wChange_destroy (GtkButton *button, gpointer label)

{
    activateChan=0;
} */


/* //Отслеживание открытия окна "Найти"
G_MODULE_EXPORT void on_wFindProject_show (GtkButton *button, gpointer label)

{
    activateFind=1;
}

//Отслеживание закрытия окна "Данные по кабелю"
G_MODULE_EXPORT void on_wFindProject_destroy (GtkButton *button, gpointer label)

{
    activateFind=0;
}
*/

// Кнопка открыть
G_MODULE_EXPORT void on_button1_clicked (GtkButton *button, gpointer label)
{
    // Определяем текущий код
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects);//Получаем модель виджета
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects); //Получаем выделенную строку
    gtk_tree_selection_get_selected(selection, &tm, &iter);//Получаем итератор и модель выделенного
    path = gtk_tree_model_get_path ( tm, &iter );  //Получаем путь к итератору
    /*int *i;
    i = gtk_tree_path_get_indices ( path ) ;
    memSel = i [ 0 ] ;*/
    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;
        // Получение кода регистрационного номера
        gtk_tree_model_get (model, &iter,
                            C_ID, &id_data,
                            -1);
        ShowChange(id_data);


        /* if (activateChan==1)
        {
           gtk_window_present(GTK_WINDOW(wChange));
        }
        else
        {
        // Определяем текущий код
        GtkTreeIter iter;
        GtkTreeModel *model = gtk_tree_view_get_model (treeview_projects);//Получаем модель виджета
        GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects); //Получаем выделенную строку
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
                               C_ID, &id_data,
                               -1);
           ShowChange(id_data);
        }*/
    }
}

// Кнопка Добавить
G_MODULE_EXPORT void on_button2_clicked (GtkButton *button, gpointer label)
{
    AddItem ();
    find_for_past ();
}


// Добавить запись по двойному клику
G_MODULE_EXPORT void on_treeview_projects_row_activated (GtkTreeView *treeview_projects, GtkTreePath *path, GtkTreeViewColumn *column, gpointer label )
{
    if (ShowQuestionAdd())
    {
    CopyPastItem();
    }

}

// Кнопка Удалить
G_MODULE_EXPORT void on_button27_clicked (GtkButton *button, gpointer label)
{
    GtkTreeIter iter;
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_projects);
    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        if (ShowQuestion())
        {
            DeleteItem ();
        }
    }
}

// Кнопка Закрыть
G_MODULE_EXPORT void on_button28_clicked (GtkButton *button, gpointer label)
{
    activateproj=0;
    gtk_widget_destroy (wProjects);
}


// Вывод таблицы
void ShowProjects()
{

    builder_projects = gtk_builder_new ();
    gtk_builder_add_from_file (builder_projects, "main.glade", NULL);
    wProjects = GTK_WIDGET(gtk_builder_get_object(builder_projects, "wProjects"));
    scrolledWindow = GTK_WIDGET(gtk_builder_get_object(builder_projects, "scrolledWindow5"));
    liststore_projects =  GTK_LIST_STORE(gtk_builder_get_object(builder_projects, "liststore_projects"));
    treeview_projects =  GTK_TREE_VIEW(gtk_builder_get_object(builder_projects, "treeview_projects"));
    gtk_window_set_default_size (GTK_WINDOW (wProjects), 1900, 800);
    gtk_window_set_position(GTK_WINDOW(wProjects), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wProjects), "Projects");// Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wProjects), "tables-icon.png", NULL); //Вывод иконки
    gtk_widget_set_events(GTK_WIDGET(scrolledWindow),GDK_BUTTON_PRESS_MASK);
    gtk_builder_connect_signals (builder_projects, NULL);

    if(LoadProjects())
    {
        ShowError();
    }
    gtk_widget_show_all(wProjects);
}

