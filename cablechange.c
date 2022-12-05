/*******************************
Ввод строки изменения данных кабеля
********************************/
#include "main.h"
int UstCable;
char ispol_name[1000];
char marka_name[1000];
char cablelength[1000];
char ustname[1000];
char typecable[1000];
char another[1000];
int CurCableLength;
int sysindex;
int sysindex1;

// Структуры данных для виджета
GtkListStore        *liststore_cable; // Буфер таблицы
GtkTreeView         *treeview_change; // Таблица
GtkTreeViewColumn   *column;    // Отображаемая колонка
GtkTreeIter         iter;       // Итератор таблицы (текущая строка)
GtkCellRenderer     *renderer;  // Рендер таблицы (текущая ячейка)
GtkComboBoxText *gIspol_name; // Исполнение ленты кабеля
GtkComboBoxText *gMarkCable; // Марка кабеля
GtkComboBoxText *gTypeCable;// Тип кабеля
GtkComboBoxText *gNazv1;// Название объекта
GtkEntry *gAnother;// Другое
GtkEntry *gNazv; // Название установки
GtkSpinButton *gDlin; // Длина кабеля

// Обозначения полей таблицы "Данные"
enum
{
    C_ID, C_REG_NUM, C_OBJ_NAME, C_MESTO_RASP, C_ZAKAZ, C_MANAG, C_PROJECTER, C_DAY, C_MONTH, C_YEAR
};

// Обозначения полей таблицы "Установленные кабели"
enum
{
    C_ID_UST_CABLE, C_MARK_CABLE, C_DLIN_CABLE, C_NAZV_UST
};

// Обозначения полей таблицы "Марки кабеля"
enum
{
    C_ID_MARK_CABLE, C_ZAKAZ_CABLE, C_ISPOLN_CABLE=5, C_TYPE_CABLE
};

// Загрузка цифровых полей
int LoadCableChange(int ID_CableLength)
{
    // Поиск нестандартного кабеля
    char* SQL0="SELECT\
    [Другое]\
    FROM [Установленный кабель]\
    WHERE [Код длины установленного кабеля] =";
    // Готовим SQL-запрос к БД
    lenstr = 0;
    lenstr+=sprintf(str, SQL0);
    lenstr+=sprintf(str+lenstr, "%d",UstCable );
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Выполнение SQL-запроса и запись в буфер таблицы
    rc = sqlite3_step(stmt);
    // Проверка пустоты строки
    if (sqlite3_column_text(stmt, 0)!='\0')
    {
        // Получаем содержимое "другое"
        sprintf(another,"%s",sqlite3_column_text(stmt, 0));
        //Освобождаем строку запроса
        sqlite3_finalize(stmt);
        //Освобождаем строку запроса
        gtk_entry_set_text(gAnother,another);
        char* SQL = "SELECT \
        [Код длины установленного кабеля],\
        [Другое],\
        [Длина установленного кабеля],\
        [Название установки],\
        [Код регистрационного номера]\
    FROM [Установленный кабель] AS a\
    WHERE [Код длины установленного кабеля] =";
        CurCableLength=ID_CableLength;
        sprintf(str, "%s%d",SQL, ID_CableLength);
        //ShowSQL(); // Тестовый вывод запроса
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK )
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowError();
            sqlite3_finalize(stmt);
            return 3;
        }
        //Выполнение SQL-запроса
        rc = sqlite3_step(stmt);
        //Получение и заполнение названия установки
        sprintf(ustname,"%s", sqlite3_column_text(stmt, C_NAZV_UST));
        gtk_entry_set_text(GTK_ENTRY(gNazv),ustname);
        //Получение и заполнение длины кабеля
        sprintf(cablelength,"%s", sqlite3_column_text(stmt, C_DLIN_CABLE));
        gtk_spin_button_set_value(gDlin, atoi(cablelength));
        //Получение марки кабеля
        sprintf(marka_name,"%s", sqlite3_column_text(stmt, C_MARK_CABLE));
        //Освобождаем строку запроса
        sqlite3_finalize(stmt);
    }
    else
    {
        //Освобождаем строку запроса
        sqlite3_finalize(stmt);
        char* SQL = "SELECT \
        [Код длины установленного кабеля],\
        [Марка кабеля],\
        [Длина установленного кабеля],\
        [Название установки],\
        [Код регистрационного номера],\
        [Исполнение кабеля],\
        [Тип кабеля]\
    FROM [Установленный кабель] AS a\
    JOIN\
    [Марки кабеля] AS c ON a.[Код марки кабеля] = c.[Код марки кабеля]\
    WHERE [Код длины установленного кабеля] =";
        //Освобождаем строку запроса
        CurCableLength=ID_CableLength;
        sprintf(str, "%s%d",SQL, ID_CableLength);
        //ShowSQL(); // Тестовый вывод запроса
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK )
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowError();
            sqlite3_finalize(stmt);
            return 3;
        }
        // Выполнение SQL-запроса
        rc = sqlite3_step(stmt);
        //Получение названия установки
        sprintf(ustname,"%s", sqlite3_column_text(stmt, C_NAZV_UST));
        gtk_entry_set_text(GTK_ENTRY(gNazv),ustname);
        //ChangeUst(ustname);
        //Получение длины кабеля
        sprintf(cablelength,"%s", sqlite3_column_text(stmt, C_DLIN_CABLE));
        gtk_spin_button_set_value(gDlin, atoi(cablelength));
        //Получение марки кабеля
        sprintf(marka_name,"%s", sqlite3_column_text(stmt, C_MARK_CABLE));
        //Получение исполнения греющей ленты кабеля
        sprintf(ispol_name,"%s", sqlite3_column_text(stmt, C_ISPOLN_CABLE));
        //Получение исполнения греющей ленты кабеля
        sprintf(typecable,"%s", sqlite3_column_text(stmt, C_TYPE_CABLE));
        //Освобождаем строку запроса
        sqlite3_finalize(stmt);
    }
    return 0;
}


// Название обогреваемого объекта
/*
int ChangeUst(char Heat_object[100])
{
    char Heat_object_Krov[100];
    sprintf(Heat_object_Krov,"%s","Кровля");
    char Heat_object_Truboprovod[100];
    sprintf(Heat_object_Truboprovod,"%s","Трубопровод");
    char Heat_object_Rezervuar[100];
    sprintf(Heat_object_Rezervuar,"%s","Резервуар");
    char Heat_object_Armatura[100];
    sprintf(Heat_object_Armatura,"%s","Арматура");
    char Heat_object_Ploshad[100];
    sprintf(Heat_object_Ploshad,"%s","Площадка");
    char Heat_object_No[100];
    sprintf(Heat_object_No,"%s","****");

    if(strcmp(Heat_object,Heat_object_Krov)==0)
    {
        gtk_combo_box_text_append_text (gNazv1, Heat_object);
        sysindex=0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(gNazv1), sysindex);
    }
    else if (strcmp(Heat_object,Heat_object_Truboprovod)==0 || strcmp(Heat_object,Heat_object_Rezervuar)==0)
    {
        gtk_combo_box_text_append_text (gNazv1, Heat_object_Truboprovod);
        gtk_combo_box_text_append_text (gNazv1, Heat_object_Rezervuar);
        if (strcmp(Heat_object,Heat_object_Truboprovod)==0)
        {
            sysindex=0;
        }
        else
        {
            sysindex=1;
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(gNazv1), sysindex);
    }

    else if (strcmp(Heat_object,Heat_object_Armatura)==0)
    {
        gtk_combo_box_text_append_text (gNazv1, Heat_object);
        sysindex=0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(gNazv1), sysindex);
    }
    else if (strcmp(Heat_object,Heat_object_Ploshad)==0)
    {
        gtk_combo_box_text_append_text (gNazv1, Heat_object);
        sysindex=0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(gNazv1), sysindex);
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(gNazv),Heat_object);
        gtk_combo_box_text_append_text (gNazv1, Heat_object_No);
        sysindex=0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(gNazv1), sysindex);
    }
    return 0;
}
*/



// Заполнение выпадающего списка "Тип кабеля"
int ChangeTypeCable()
{
    char* SQL2 = "SELECT DISTINCT [Тип кабеля]\
    FROM [Марки кабеля]";
    sprintf(str, "%s",SQL2);
    // ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL2, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Цикл по SQL-запросу и запись в выпадающий список "Тип кабеля"
    sysindex=-1;
    sysindex1=0;
    char buf0[1000];
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf0,"%s", sqlite3_column_text(stmt, 0));
        gtk_combo_box_text_append_text(gTypeCable, buf0);
        if(strcmp(buf0,typecable)==0)
        {
            sysindex1=sysindex;
        }
    }
    // Установка активного элемента
    gtk_combo_box_set_active(GTK_COMBO_BOX(gTypeCable),sysindex1);
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "ИСПОЛНЕНИЕ КАБЕЛЯ"
int ChangeIspolnCable()
{
    // Очищаем выпадающий список
    gtk_combo_box_text_remove_all(gIspol_name);
    // Получение текста активного элемента "Тип кабеля"
    sprintf(typecable, "%s%s%s","'", gtk_combo_box_text_get_active_text(gTypeCable),"'");
    // Заполнение выпадающего списка "Исполнение кабеля"
    char* SQL2 = "SELECT DISTINCT [Исполнение кабеля]\
    FROM [Марки кабеля]\
    WHERE [Тип кабеля]=";
    sprintf(str, "%s%s",SQL2,typecable);
    // ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Цикл по SQL-запросу и запись в выпадающий список "Исполнение кабеля"
    sysindex=-1;
    sysindex1=0;
    char buf0[1000];
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf0,"%s", sqlite3_column_text(stmt, 0));
        gtk_combo_box_text_append_text(gIspol_name, buf0);
        if(strcmp(buf0,ispol_name)==0)
        {
            sysindex1=sysindex;
        }
    }
    // Установка активного элемента
    gtk_combo_box_set_active(GTK_COMBO_BOX(gIspol_name),sysindex1);
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "МАРКА КАБЕЛЯ"
int ChangeMarkCable()
{
    // Очищаем выпадающий список
    gtk_combo_box_text_remove_all(gMarkCable);
    // Получение текста активного элемента "Исполнение греющей ленты"
    sprintf(ispol_name, "%s%s%s","'", gtk_combo_box_text_get_active_text(gIspol_name),"'");
    // Создание строки запроса к БД
    char* SQL3="SELECT [Код марки кабеля],\
       [Марка кабеля],\
       [Исполнение кабеля]\
        FROM [Марки кабеля]\
        WHERE [Исполнение кабеля] =";
    sprintf(str, "%s%s",SQL3, ispol_name);
    // Подготовка запроса и чтение из БД
    //ShowSQL(); // Тестовый вывод запроса
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK )
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Цикл по SQL-запросу и запись в выпадающий список "Марка кабеля"
    sysindex=-1;
    sysindex1=0;
    char buf1[1000];
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf1,"%s", sqlite3_column_text(stmt, C_ZAKAZ_CABLE));
        gtk_combo_box_text_append_text(gMarkCable, buf1);
        if(strcmp(buf1,marka_name)==0)
        {
            sysindex1=sysindex;
        }
    }
    // Установка активного элемента
    gtk_combo_box_set_active(GTK_COMBO_BOX(gMarkCable),sysindex1);
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// Переключение между группами марок кабелей
G_MODULE_EXPORT void on_comboboxtext4_changed (GtkComboBox *ComboBox, gpointer label)
{
    ChangeMarkCable();
}

// Переключение между группами марок кабелей
G_MODULE_EXPORT void on_comboboxtext_type_changed (GtkComboBox *ComboBox, gpointer label)
{
    ChangeIspolnCable();
}


// СОХРАНЕНИЕ ИЗМЕНЕННОЙ СТРОКИ ТАБЛИЦЫ "УСТАНОВЛЕННЫЙ КАБЕЛЬ"
void SaveCable()
{
    // Получение из combo_box марки кабеля
    sprintf(marka_name, "%s%s%s","'", gtk_combo_box_text_get_active_text(gMarkCable),"'");
    // Получение из spinbutton длины кабеля
    sprintf(cablelength, "%d", gtk_spin_button_get_value_as_int(gDlin));
    // Получение из entry названия установки
    sprintf(ustname, "%s%s%s","'", gtk_entry_get_text(gNazv),"'");
    // Получение из gAnother числа знаков
    int empty=gtk_entry_get_text_length(gAnother);
    // Формирование строки запроса к базе данных
    if (empty!=0)
    {
        sprintf(another, "%s%s%s","'", gtk_entry_get_text(gAnother),"'");
        lenstr = 0;
        lenstr+=sprintf(str, "UPDATE [Установленный кабель] SET [Длина установленного кабеля]=%d, ", atoi(cablelength));
        lenstr+=sprintf(str+lenstr, "[Название установки] =%s,", ustname);
        lenstr+=sprintf(str+lenstr, "[Код марки кабеля] =%d,", 0);
        lenstr+=sprintf(str+lenstr, "Другое =%s ", another);
        lenstr+=sprintf(str+lenstr, "WHERE [Код длины установленного кабеля]=%d", CurCableLength);
        // Записываем в БД
        if (sqlite3_exec(db, str, 0, 0, &err))
        {
            sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
            ShowError();
            sqlite3_free(err);
            sqlite3_finalize(stmt);
            return;
        }
        //ShowSQL();// Тестовый запрос
        sqlite3_finalize(stmt);
    }
    else
    {
        // Подготовка запроса и чтение из БД
        sprintf(str, "SELECT [Код марки кабеля]\
            FROM [Марки кабеля]\
            WHERE [Марка кабеля] = %s", marka_name);
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowError();
            sqlite3_finalize(stmt);
            return;
        }
        // Выполнение запроса
        rc = sqlite3_step(stmt);
        // Получение кода марки стандартного кабеля
        int ID_markcable=sqlite3_column_int(stmt, C_ID_MARK_CABLE);
        // Освобождаем строку запроса
        sqlite3_finalize(stmt);
        // Формирование запроса
        lenstr = 0;
        lenstr+=sprintf(str, "UPDATE [Установленный кабель] SET [Длина установленного кабеля]=%d, ", atoi(cablelength));
        lenstr+=sprintf(str+lenstr, "[Название установки] =%s,", ustname);
        lenstr+=sprintf(str+lenstr, "[Код марки кабеля] =%d, ", ID_markcable);
        lenstr+=sprintf(str+lenstr, "Другое =%s ","NULL" );
        lenstr+=sprintf(str+lenstr, "WHERE [Код длины установленного кабеля]=%d", CurCableLength);
        // Запись в БД
        if (sqlite3_exec(db, str, 0, 0, &err))
        {
            sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
            ShowError();
            sqlite3_free(err);
            sqlite3_finalize(stmt);
            return;
        }
        //ShowSQL();// Тестовый запрос
        // Освобождаем строку запроса
        sqlite3_finalize(stmt);
    }
}

// Обновить запись
int ReloadCable(int UstCable)
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_change); //Получаем модель виджета
    gtk_tree_model_get_iter (model, &iter, path1); //Получаем итератор из модели по пути path
    // Запрос по нестандартной марки оборудования
    char* SQL0="SELECT\
    [Другое]\
    FROM [Установленный кабель]\
    WHERE [Код длины установленного кабеля] =";
    // Готовим SQL-запрос к БД
    lenstr = 0;
    lenstr+=sprintf(str, SQL0);
    lenstr+=sprintf(str+lenstr, "%d",UstCable );
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Выполнение SQL-запроса
    rc = sqlite3_step(stmt);
    //Получение данных о нестандартном кабеле
    if (sqlite3_column_text(stmt, 0)!='\0')// Проверка зхаполненности ячейки
    {
        // Освобождение строки запроса
        sqlite3_finalize(stmt);
        char* SQL = "SELECT\
    [Код длины установленного кабеля],\
    [Другое],\
    [Длина установленного кабеля],\
    [Название установки]\
    FROM [Установленный кабель]\
    WHERE [Код длины установленного кабеля] =";
        // Готовим SQL-запрос к БД
        lenstr = 0;
        lenstr+=sprintf(str, SQL);
        lenstr+=sprintf(str+lenstr, "%d", UstCable);
        ///ShowSQL(); // Тестовый вывод запроса
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowError();
            sqlite3_finalize(stmt);
            return 3;
        }
        // Выполнение SQL-запроса и запись в буфер таблицы
        rc = sqlite3_step(stmt);
        gtk_list_store_set (liststore_cable, &iter,
                            C_MARK_CABLE, sqlite3_column_text(stmt, C_MARK_CABLE),
                            C_DLIN_CABLE, sqlite3_column_int(stmt, C_DLIN_CABLE),
                            C_NAZV_UST, sqlite3_column_text(stmt, C_NAZV_UST),-1);
        // Освобождаем строку запроса
        sqlite3_finalize(stmt);
    }
    else // Получение данных о стандартном кабеле
    {
        // Освобождаем строку запроса
        sqlite3_finalize(stmt);
        char* SQL = "SELECT\
    [Код длины установленного кабеля],\
    [Марка кабеля],\
    [Длина установленного кабеля],\
    [Название установки]\
    FROM [Установленный кабель] AS a\
    JOIN\
    [Марки кабеля] AS c ON a.[Код марки кабеля] = c.[Код марки кабеля]\
    WHERE [Код длины установленного кабеля] =";
        // Готовим SQL-запрос к БД
        lenstr = 0;
        lenstr+=sprintf(str, SQL);
        lenstr+=sprintf(str+lenstr, "%d", UstCable);
        ///ShowSQL(); // Тестовый вывод запроса
        // Подготовка запроса
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowError();
            sqlite3_finalize(stmt);
            return 3;
        }
        // Выполнение SQL-запроса и запись в буфер таблицы
        rc = sqlite3_step(stmt);
        gtk_list_store_set (liststore_cable, &iter,
                            C_MARK_CABLE, sqlite3_column_text(stmt, C_MARK_CABLE),
                            C_DLIN_CABLE, sqlite3_column_int(stmt, C_DLIN_CABLE),
                            C_NAZV_UST, sqlite3_column_text(stmt, C_NAZV_UST),-1);
        // Освобождаем строку запроса
        sqlite3_finalize(stmt);
    }
    return 0;
}

// Кнопка Закрыть
G_MODULE_EXPORT void on_button30_clicked (GtkButton *button, gpointer label)
{
    SaveCable();
    ReloadCable(UstCable);
    //activateCable=0;
    gtk_widget_destroy(wCable);
}

// Вывод окна
void ShowCable(int ID_UstCable)
{
    UstCable=ID_UstCable;
    // Создание билдера
    bCabChange = gtk_builder_new ();
    gtk_builder_add_from_file (bCabChange, "main.glade", NULL);
    // Создание Виджетов
    wCable = GTK_WIDGET(gtk_builder_get_object(bCabChange, "wCable"));
    gNazv = GTK_ENTRY(gtk_builder_get_object(bCabChange, "entry3"));
    gDlin = GTK_SPIN_BUTTON(gtk_builder_get_object(bCabChange, "spinbutton_cable_length"));
    gNazv1 = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bCabChange, "comboboxtext_nazv"));
    gIspol_name = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bCabChange, "comboboxtext4"));
    gMarkCable = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bCabChange, "comboboxtext5"));
    gTypeCable = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bCabChange, "comboboxtext_type"));
    gAnother = GTK_ENTRY(gtk_builder_get_object(bCabChange, "entry_another"));
    gtk_window_set_default_size (GTK_WINDOW (wCable), 850, 250);
    gtk_window_set_position(GTK_WINDOW(bCabChange), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wCable), "Cables");// Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wCable), "tables-icon.png", NULL); //Вывод иконки
    gtk_window_set_modal(GTK_WINDOW(wCable),TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(wCable),GTK_WINDOW(wChange));
    gtk_builder_connect_signals (bCabChange, NULL);
    if(LoadCableChange(UstCable))
    {
        ShowError();
    }
    ChangeTypeCable();
    gtk_widget_show_all(wCable);
}
