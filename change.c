#include "main.h"
#include <stdbool.h>
#include <locale.h>
#define _LOCALE_TO_UTF8(str) g_locale_to_utf8((const char *)(str), -1, NULL, NULL, NULL)
#define _UTF8_TO_LOCALE(str) g_locale_from_utf8((const char *)(str), -1, NULL, NULL, NULL)
#define _UTF8(str) _LOCALE_TO_UTF8((str))
#define _LOCALE(str) _UTF8_TO_LOCALE((str))
#define _U(str) _UTF8((str))
#define _L(str) _LOCALE((str)) )

// Переменные даты
int gDay, gMonth, gYear;
// Текущий код номера проекта
int ID_proj;
// Структуры данных для виджета
GtkListStore        *liststore_cable;     // Буфер таблицы
GtkTreeModel        *liststore_cable1;
GtkTreeView         *treeview_change;      // Таблица
GtkTreeViewColumn   *column;    // Отображаемая колонка
GtkTreeIter         iter;       // Итератор таблицы (текущая строка)
GtkCellRenderer     *renderer;  // Рендер таблицы (текущая ячейка)
GtkComboBoxText *gSystem; // Буквенный индекс системы
GtkComboBoxText *gManager; // Менеджер
GtkComboBoxText *gProjecter; // Проектировщик
GtkSpinButton   *gPorNom; // Порядковый номер
GtkSpinButton   *gPorNomProj;// Порядковый номер в проекте
GtkEntry *gObjname; // Название объекта
GtkEntry *gObjsituate; // Географическое расположение объектов
GtkEntry *gZakazpname;// Название заказчика
GtkEntry *gPrim; // Примечания
// Обозначения полей таблицы "Данные"
enum
{
    C_ID, C_REG_NUM, C_OBJ_NAME, C_MESTO_RASP, C_ZAKAZ, C_MANAG, C_PROJECTER, C_DAY, C_MONTH, C_YEAR, C_PRIM
};
// Обозначения полей таблицы "Менеджеры"
enum
{
    C_ID_MAN, C_FAMIL_MANAG, C_NAME_MANAG, C_KOD_SOTRUDNICHESTVA_MAN
};
// Обозначения полей таблицы "Проектировщики"
enum
{
    C_ID_PROJ, C_FAMIL_PROJ, C_NAME_PROJ, C_KOD_SOTRUDNICHESTVA_PROJ
};
// Обозначения полей таблицы "Установленные кабели"
enum
{
    C_ID_CABLE, C_MARK_CABLE, C_DLIN_CABLE, C_NAZV_UST
};

// Загрузка из БД в буфер виджета
int LoadChange(int proj)
{
    //ID_proj=proj;
    // Запись в цифровые виджеты
    char buf[100];
    char buf2[100];
    /*  char* SQL = "SELECT [Код накладной], Номер,  \
                    strftime('%%d', Дата), \
                    strftime('%%m', Дата), \
                    strftime('%%Y', Дата) \
                    FROM Накладные  \
                    WHERE [Код накладной] = ";
    */
    char* SQL = "SELECT [Код регистрационного номера],[Регистрационный номер],[Название объекта], [Географическое расположение],[Название заказчика], Менеджер, Проектировщик, \
    strftime('%d',Дата), \
    strftime('%m',Дата), \
    strftime('%Y',Дата), \
    Примечания\
    FROM Данные WHERE [Код регистрационного номера] = ";
    sprintf(str, "%s%d",SQL, proj);
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
    //Получение из базы данных названия объекта
    char objname[1000];
    sprintf(objname,"%s", sqlite3_column_text(stmt, C_OBJ_NAME));
    gtk_entry_set_text(GTK_ENTRY(gObjname),objname);
    //Получение из базы данных географического расположения
    char objsituate[1000];
    sprintf(objsituate,"%s", sqlite3_column_text(stmt, C_MESTO_RASP));
    gtk_entry_set_text(GTK_ENTRY(gObjsituate),objsituate);
    //Получение из базы данных названия заказчика
    char zakazname[1000];
    sprintf(zakazname,"%s", sqlite3_column_text(stmt, C_ZAKAZ));
    gtk_entry_set_text(GTK_ENTRY(gZakazpname),zakazname);
    //Получение из базы данных имени менеджера
    char man_name[1000];
    sprintf(man_name,"%s", sqlite3_column_text(stmt, C_MANAG));
    //Получение из базы данных имени проектировщика
    char proj_name[1000];
    sprintf(proj_name,"%s", sqlite3_column_text(stmt, C_PROJECTER));
    //Получение из базы данных даты
    if (sqlite3_column_text(stmt, C_DAY) > 0)
    {
        gDay = atoi((char*) sqlite3_column_text(stmt, C_DAY));
        gMonth = atoi((char*)sqlite3_column_text(stmt, C_MONTH))-1;
        gYear = atoi((char*)sqlite3_column_text(stmt, C_YEAR));
        gtk_calendar_select_month(gCalendar, gMonth, gYear);
        gtk_calendar_select_day(gCalendar, gDay);
    }
    else
    {
        gDay=1;
        gMonth=1;
        gYear=2014;
    }
    //Получение из базы данных примечаний
    char prim[1000];
    sprintf(prim,"%s", sqlite3_column_text(stmt, C_PRIM));
    gtk_entry_set_text(GTK_ENTRY(gPrim),prim);
    //Получение из базы данных регистрационного номера проекта
    sprintf(buf,"%s", sqlite3_column_text(stmt, C_REG_NUM));
    //Освобождаем строку запроса
    sqlite3_finalize(stmt);
    //Получение из базы данных номера системы в проекте бумажном
    memcpy( buf2, &buf[16], 2 );
    buf2[2] = '\0';
    gtk_spin_button_set_value(gPorNomProj, atoi(buf2));
    //Получение из базы данных порядкового номера системы
    memcpy( buf2, &buf[10], 5 );
    buf2[5] = '\0';
    gtk_spin_button_set_value(gPorNom, atoi(buf2));
    //Получение из базы данных типа системы
    memcpy( buf2, &buf, 4 );
    buf2[4] = '\0';
    sprintf(str, "%s",buf2);
    // ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "ТИП СИСТЕМЫ"
    memset(buf, 0, sizeof(buf));
    memset(buf2, 0, sizeof(buf2));
    char tm[100]="ТМ";
    char ts[100]="ТС";
    char td[100]="ТД";
    char tch[100]="ТЧ";
    int sysindex;
    if (strcmp(str,tm)==0)
    {
        sysindex=0;
    }
    else if(strcmp(str,ts)==0)
    {
        sysindex=1;
    }
    else if(strcmp(str,td)==0)
    {
        sysindex=2;
    }
    else if(strcmp(str,tch)==0)
    {
        sysindex=3;
    }
    else
    {
        sysindex=-1;
    }
    gtk_combo_box_text_append_text (gSystem, tm);
    gtk_combo_box_text_append_text (gSystem, ts);
    gtk_combo_box_text_append_text (gSystem, td);
    gtk_combo_box_text_append_text (gSystem, tch);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gSystem), sysindex);

    // ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "МЕНЕДЖЕРЫ"
    char* SQL2 = "SELECT [Код сотрудника],Фамилия,Должность,Сотрудничество FROM Сотрудники WHERE Должность='Менеджер' ORDER BY Фамилия";
    sprintf(str, "%s",SQL2);
    //ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL2, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Цикл по SQL-запросу и запись в выпадающий список "Менеджеры"
    sysindex=-1; // Подсчет заполненных строк списка (с нулём)
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf,"%s", sqlite3_column_text(stmt, C_FAMIL_MANAG));
        if(strcmp(buf,man_name)==0)
        {
            // Установка активного элемента
            gtk_combo_box_text_append_text(gManager, buf);
            gtk_combo_box_set_active(GTK_COMBO_BOX(gManager),sysindex);
            continue;
        }
        else if (sqlite3_column_int(stmt,C_KOD_SOTRUDNICHESTVA_MAN)==0)// Определение статуса сотрудничества
        {
            sysindex--; // Строчка списка не заполнилась
            continue;
        }
        gtk_combo_box_text_append_text(gManager, buf);

    }
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(gManager))==-1) // Устанавливаем активный элемент если не установлен
    {
        sysindex++;
        gtk_combo_box_text_append_text(gManager, "Не указано");
        gtk_combo_box_set_active(GTK_COMBO_BOX(gManager),sysindex);
    }
    else
    {
        gtk_combo_box_text_append_text(gManager, "Не указано");
    }

    // Если ошибка
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);

    // ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "ПРОЕКТИРОВЩИКИ"
    char* SQL3 = "SELECT [Код сотрудника],Фамилия,Должность,Сотрудничество FROM Сотрудники WHERE Должность='Проектировщик' ORDER BY Фамилия";
    sprintf(str, "%s",SQL3);
    //ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL3, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }

    sysindex=-1;// Подсчет заполненных строк списка (с нулём)
    // Цикл по SQL-запросу и запись в выпадающий список "Проектировщики"
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf,"%s", sqlite3_column_text(stmt, C_FAMIL_PROJ));
        if(strcmp(buf,proj_name)==0)
        {
            // Установка активного элемента
            gtk_combo_box_text_append_text(gProjecter, buf);
            gtk_combo_box_set_active(GTK_COMBO_BOX(gProjecter),sysindex);
            continue;
        }
        else if (sqlite3_column_int(stmt,C_KOD_SOTRUDNICHESTVA_PROJ)==0)// Определение статуса сотрудничества
        {
            sysindex--;// Строчка списка не заполнилась
            continue;
        }
        gtk_combo_box_text_append_text(gProjecter, buf);
    }

    if (gtk_combo_box_get_active(GTK_COMBO_BOX(gProjecter))==-1) // Устанавливаем активный элемент если не установлен
    {
        sysindex++;
        gtk_combo_box_text_append_text(gProjecter, "Не указано");
        gtk_combo_box_set_active(GTK_COMBO_BOX(gProjecter),sysindex);
    }
    else
    {
        gtk_combo_box_text_append_text(gProjecter, "Не указано");
    }


    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    // Завершение модуля
    return 0;
}

// Загрузка таблицы из БД в буфер виджета
int LoadUstCabel(int proj)
{
    char* SQL = "SELECT \
    [Код длины установленного кабеля],\
    [Марка кабеля],\
    [Длина установленного кабеля],\
    [Название установки],\
    [Код регистрационного номера]\
    FROM [Установленный кабель] AS a\
    JOIN\
    [Марки кабеля] AS c ON a.[Код марки кабеля] = c.[Код марки кабеля]\
    WHERE [Код регистрационного номера] =";
    // Готовим SQL-запрос к БД
    sprintf(str, "%s%d",SQL, proj);
    //ShowSQL(); // Тестовый вывод запроса
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
        gtk_list_store_append (liststore_cable, &iter);
        gtk_list_store_set (liststore_cable, &iter,
                            C_ID_CABLE, sqlite3_column_int(stmt, C_ID_CABLE),
                            C_MARK_CABLE, sqlite3_column_text(stmt, C_MARK_CABLE),
                            C_DLIN_CABLE, sqlite3_column_int(stmt, C_DLIN_CABLE),
                            C_NAZV_UST, sqlite3_column_text(stmt, C_NAZV_UST),
                            -1);
    }
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    char* SQL0 = "SELECT \
    [Код длины установленного кабеля],\
    Другое,\
    [Длина установленного кабеля],\
    [Название установки],\
    [Код регистрационного номера]\
    FROM [Установленный кабель]\
    WHERE [Другое] IS NOT NULL\
    AND [Код регистрационного номера] =";
    // Готовим SQL-запрос к БД
    sprintf(str, "%s%d",SQL0, proj);
    // ShowSQL(); // Тестовый вывод запроса
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
        gtk_list_store_append (liststore_cable, &iter);
        gtk_list_store_set (liststore_cable, &iter,
                            C_ID_CABLE, sqlite3_column_int(stmt, C_ID_CABLE),
                            C_MARK_CABLE, sqlite3_column_text(stmt, C_MARK_CABLE),
                            C_DLIN_CABLE, sqlite3_column_int(stmt, C_DLIN_CABLE),
                            C_NAZV_UST, sqlite3_column_text(stmt, C_NAZV_UST),
                            -1);
    }
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

// Добавить строку
int AddUstCabel (int proj)
{
    // Вставка новой записи в базу
    char* SQL="INSERT INTO [Установленный кабель] ([Код регистрационного номера]) VALUES (";
    // Готовим SQL-запрос к БД
    sprintf(str, "%s%d%s",SQL, proj,");");
    //ShowSQL(); // Тестовый вывод запроса
    // Записываем в БД
    if (sqlite3_exec(db, str, 0, 0, &err))
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
        ShowError();
        sqlite3_free(err);
        return 1;
    }
    // Получение последней вставленной строки
    char* SQL2 = "SELECT \
    [Код длины установленного кабеля],\
    [Марка кабеля],\
    [Длина установленного кабеля],\
    [Название установки],\
    [Код регистрационного номера]\
    FROM [Установленный кабель] AS a\
    JOIN\
    [Марки кабеля] AS c ON a.[Код марки кабеля] = c.[Код марки кабеля]\
    ORDER BY [Код длины установленного кабеля] DESC LIMIT 1";
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
    gtk_list_store_append (liststore_cable, &iter);
    gtk_list_store_set (liststore_cable, &iter,
                        C_ID_CABLE, sqlite3_column_int(stmt, C_ID_CABLE),
                        C_MARK_CABLE, sqlite3_column_text(stmt, C_MARK_CABLE),
                        C_DLIN_CABLE, sqlite3_column_int(stmt, C_DLIN_CABLE),
                        C_NAZV_UST, sqlite3_column_text(stmt, C_NAZV_UST),
                        -1);
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// Удаление записи
void DeleteUstCabel()
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (treeview_change);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_change);
    if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
        gint id_data;
        // Получение кода
        gtk_tree_model_get (model, &iter,
                            C_ID_CABLE, &id_data,
                            -1);
        // Удаление строки из буфера
        gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
        // Удаление записи из БД
        lenstr = 0;
        lenstr+=sprintf(str, "DELETE FROM [Установленный кабель] WHERE [Код длины установленного кабеля]=");
        lenstr+=sprintf(str+lenstr, "%d", id_data);
        lenstr+=sprintf(str+lenstr, ";");
        ///ShowSQL(); // Тестовый вывод запроса
        // Записываем в БД
        if (sqlite3_exec(db, str, 0, 0, &err))
        {
            sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
            ShowError();
            sqlite3_free(err);
            sqlite3_finalize(stmt);
            return;
        }
    }
}

// Сохранение записи
void SaveChange(int proj)
{
    char CurDate[1000];
    char regnom[1000];
    // Получение из combo_box типа системы
    char System[1000];
    sprintf(System, "%s", gtk_combo_box_text_get_active_text(gSystem));
    // Получение из combo_box номера системы в проекте бумажном
    int PorNomProj=gtk_spin_button_get_value_as_int (gPorNomProj);
    // Получение из combo_box порядкового номера системы
    int PorNom=gtk_spin_button_get_value_as_int (gPorNom);
    // Сборка регистрационного номера
    lenstr = 0;
    lenstr+=sprintf(regnom, "'%s-ПО-", System);
    lenstr+=sprintf(regnom+lenstr, "%d-", PorNom);
    lenstr+=sprintf(regnom+lenstr, "%d'", PorNomProj);
    // Получение название объекта
    char objnameS[300];
    sprintf(objnameS, "'%s'", gtk_entry_get_text(gObjname));
    // Получение расположения объекта
    char objsituateS[300];
    sprintf(objsituateS, "'%s'", gtk_entry_get_text(gObjsituate));
    // Получение названия заказчика
    char zakazpnameS[300];
    sprintf(zakazpnameS, "'%s'",gtk_entry_get_text(gZakazpname));
    // Получение примечаний
    char primS[1000];
    sprintf(primS, "'%s'",gtk_entry_get_text(gPrim));
    //Получение из выпадающего списка фамилию менеджера
    char man_nameS[100];
    sprintf(man_nameS, "'%s'", gtk_combo_box_text_get_active_text(gManager));
    // Получение из выпадающего списка фамилию проектировщика
    char proj_nameS[100];
    sprintf(proj_nameS, "'%s'", gtk_combo_box_text_get_active_text(gProjecter));
    // Переменные GTK для хранения данных даты
    guint year_data;
    guint month_data;
    guint day_data;
    // Получение даты из виджета
    gtk_calendar_get_date(gCalendar, &year_data, &month_data, &day_data);
    // Запись даты в переменную
    sprintf(CurDate,"%4d-%02d-%02d", year_data, ++month_data, day_data);
    //sprintf(CurDate,"%02d-%02d-%4d", day_data, ++month_data, year_data);

    /*lenstr = 0;
    lenstr+=sprintf(str, "UPDATE Данные SET [Регистрационный номер]=%s", regnom);
    lenstr+=sprintf(str+lenstr, "WHERE [Код регистрационного номера]=%d", proj);*/

    // Готовим SQL-запрос
    lenstr = 0;
    lenstr+=sprintf(str, "UPDATE Данные SET [Регистрационный номер]=%s,", regnom);
    lenstr+=sprintf(str+lenstr, "[Название объекта]=%s,[Географическое расположение]=%s,[Название заказчика]=%s,Менеджер=%s,Проектировщик=%s,",objnameS,objsituateS,zakazpnameS,man_nameS,proj_nameS);
    lenstr+=sprintf(str+lenstr, "Дата='%s', Примечания=%s", CurDate, primS);
    lenstr+=sprintf(str+lenstr, " WHERE [Код регистрационного номера]=%d", proj);
    //ShowSQL(); // Тестовый вывод запроса
    // Записываем полученные данные в БД
    if (sqlite3_exec(db, str, 0, 0, &err))
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
        ShowError();
        sqlite3_free(err);
        sqlite3_finalize(stmt);
        return;
    }
}

// Вывод на печать
static void DrawProject(GtkPrintOperation *op, GtkPrintContext *context, int page_nr)
{
    //setlocale(LC_ALL, "Russian");
    cairo_t *cr;
    gchar buf[1000];
    gchar buf1[1000];
    gchar bufr[1000];
    gchar bufrr[1000];
    cr = gtk_print_context_get_cairo_context (context);
    // Получение из combo_box типа системы
    char SystemP[1000];
    sprintf(SystemP, "%s", gtk_combo_box_text_get_active_text(gSystem));
    // Получение из combo_box номера системы в проекте бумажном
    int PorNomProjP=gtk_spin_button_get_value_as_int(gPorNomProj);
    // Получение из combo_box порядкового номера системы
    int PorNomP=gtk_spin_button_get_value_as_int(gPorNom);
    // Сборка регистрационного номера
    char regnomP[1000];
    lenstr = 0;
    lenstr+=sprintf(regnomP, "%s-ПО-", SystemP);
    lenstr+=sprintf(regnomP+lenstr, "%d-", PorNomP);
    lenstr+=sprintf(regnomP+lenstr, "%d", PorNomProjP);
    // Получение название объекта
    gchar objnameP[1000];
    sprintf(objnameP, "%s", gtk_entry_get_text(gObjname));
    // Получение расположения объекта
    gchar objsituateP[1000];
    sprintf(objsituateP, "%s", gtk_entry_get_text(gObjsituate));
    // Получение названия заказчика
    char zakazpnameP[1000];
    sprintf(zakazpnameP, "%s",gtk_entry_get_text(gZakazpname));
    // Получение примечаний
    char primP[1000];
    sprintf(primP, "%s",gtk_entry_get_text(gPrim));
    // Получение из выпадающего списка фамилию менеджера
    char man_nameP[100];
    sprintf(man_nameP, "%s", gtk_combo_box_text_get_active_text(gManager));
    // Получение из выпадающего списка фамилию проектировщика
    char proj_nameP[100];
    sprintf(proj_nameP, "%s", gtk_combo_box_text_get_active_text(gProjecter));
    // Переменные GTK для хранения данных даты
    guint year_data;
    guint month_data;
    guint day_data;
    // Получение даты из виджета
    gtk_calendar_get_date(gCalendar, &year_data, &month_data, &day_data);
    // Запись даты в переменную
    char CurDateP[1000];
    sprintf(CurDateP,"%02d-%02d-%4d", day_data, ++month_data, year_data);
    // Выводим заголовок
    cairo_move_to (cr, 80.0, 10.0);
    cairo_set_font_size (cr, 6.0);
    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text (cr, "Данные проекта");
    cairo_stroke(cr);
    // Выводим регистрационный номер
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 20);
    cairo_set_font_size (cr, 4.0);
    sprintf(bufr, "Регистрационный номер:  %s", regnomP);
    cairo_show_text (cr, bufr);
    cairo_stroke(cr);
    // Задаем отступ при печати
    int stepprint=5;
    // Выводим название объекта
    if (strlen(objnameP)<77)
    {
        sprintf(buf, "Объект:  %s",objnameP);
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, 25);
        cairo_set_font_size (cr, 4.0);
        cairo_show_text (cr, buf);
        cairo_stroke(cr);
        stepprint=0;
    }
    else
    {
        memcpy(buf, &objnameP[0], 77);
        buf[77]='\0';
        memcpy(buf1, &objnameP[77], 86 );
        buf1[86]='\0';
        sprintf(bufr, "Объект:  %s",buf);
        sprintf(bufrr, "%s",buf1);
        /// sprintf(buf, "Объект - %s",objnameP);
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, 25);
        cairo_set_font_size (cr, 4.0);
        cairo_show_text (cr, bufr);
        cairo_move_to (cr, 10.0, 30);
        cairo_show_text (cr, bufrr);
        cairo_stroke(cr);
    }
    // Выводим географическое расположение
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 30+stepprint);
    cairo_set_font_size (cr, 4.0);
    sprintf(buf, "Расположение:  %s", objsituateP);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);
    // Выводим название заказчика
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 35+stepprint);
    cairo_set_font_size (cr, 4.0);
    sprintf(buf, "Заказчик:  %s", zakazpnameP);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);
    // Выводим название менеджер
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 40+stepprint);
    cairo_set_font_size (cr, 4.0);
    sprintf(buf, "Менеджер:  %s", man_nameP);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);
    // Выводим название проектировщик
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 45+stepprint);
    cairo_set_font_size (cr, 4.0);
    sprintf(buf, "Проектировщик:  %s", proj_nameP);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);
    // Выводим дату
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 50+stepprint);
    cairo_set_font_size (cr, 4.0);
    sprintf(buf, "Дата:  %s", CurDateP);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);
    // Выводим примечания
    if (strlen(primP)<77)
    {
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, 55+stepprint);
        cairo_set_font_size (cr, 4.0);
        sprintf(buf, "Примечания:  %s", primP);
        cairo_show_text (cr, buf);
        cairo_stroke(cr);
    }
    else
    {
        memcpy(buf, &primP[0], 73);
        buf[73]='\0';
        memcpy(buf1, &primP[74], 86);
        buf1[86]='\0';
        sprintf(bufr, "Примечания:  %s",buf);
        sprintf(bufrr, "%s",buf1);
        //sprintf(buf, "Объект - %s",objnameP);
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, 55+stepprint);
        cairo_set_font_size (cr, 4.0);
        cairo_show_text (cr, bufr);
        cairo_move_to (cr, 10.0, 60+stepprint);
        cairo_show_text (cr, bufrr);
        cairo_stroke(cr);
        stepprint+=5;
    }
    // Выводим данные по установленному кабелю
    cairo_move_to (cr, 75.0, 60+stepprint);
    cairo_set_font_size (cr, 5.0);
    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text (cr, "Установленный кабель");
    cairo_stroke(cr);
    // На случай переноса кнопки в проекты
    /*// Текст запроса к БД
        char* SQL = "SELECT \
            [Марка кабеля],\
            [Длина установленного кабеля],\
            [Название установки]\
            FROM [Установленный кабель] AS a\
            JOIN [Марки кабеля] AS c ON a.[Код марки кабеля] = c.[Код марки кабеля]\
            WHERE [Код регистрационного номера] =";
    	// Подготовка запроса к БД
        sprintf(str, "%s%d",SQL, ID_proj);
    //   ShowSQL(); // Тестовый вывод запроса
        if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK )
        {
            sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
            ShowSQL();
            sqlite3_finalize(stmt);
        }*/
    // Сдвиг расположения записи при рисовании
    stepprint=65+stepprint;
    // Создаем итератор
    GtkTreeIter iter;
    // Проверяем наличие данных в первой строке
    gboolean valid = gtk_tree_model_get_iter_first (liststore_cable1, &iter);
    // Цикл "Пока строка не пуста"
    while (valid)
    {
        // Проходим через список, читаем каждую строку
        gchar *str_data;
        gchar *str3_data;
        gint int_data;
        // Удостоверьтесь что вы закончили вызов gtk_tree_model_get() значением '-1'
        // Получаем содержимое ячеек по адресам столбцов номеру итератора(строка)
        gtk_tree_model_get (liststore_cable1, &iter,
                            1, &str_data,
                            3, &str3_data,
                            2, &int_data,
                            -1);
        // Выводим марку кабеля
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, stepprint);
        cairo_set_font_size (cr, 4.0);
        sprintf(buf, "Обогреваемый объект:  %s", str3_data);
        cairo_show_text (cr, buf);
        cairo_stroke(cr);
        // Выводим марку кабеля
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, stepprint+=5);
        cairo_set_font_size (cr, 4.0);
        sprintf(buf, "Марка кабеля:  %s", str_data);
        cairo_show_text (cr, buf);
        cairo_stroke(cr);
        // Выводим длину кабеля
        cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_move_to (cr, 10.0, stepprint+=5);
        cairo_set_font_size (cr, 4.0);
        sprintf(buf, "Длина:  %d м", int_data);
        cairo_show_text (cr, buf);
        cairo_stroke(cr);
        // Разделяем данные чертой
        cairo_move_to (cr, 10.0, stepprint+=3);
        cairo_set_line_width(cr,1);
        cairo_line_to(cr,60,stepprint);
        cairo_stroke(cr);
        stepprint+=5;
        // Получение следующего итератора
        valid = gtk_tree_model_iter_next (liststore_cable1, &iter);
    }
}

// Печать данных проекта
void PrintSotrudnik()
{
    GtkPrintOperation *op;
    op = gtk_print_operation_new ();
    gtk_print_operation_set_n_pages (op, 1);
    gtk_print_operation_set_unit (op, GTK_UNIT_MM);
    g_signal_connect (op, "draw_page", G_CALLBACK (DrawProject), NULL);
    gtk_print_operation_run (op, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (wChange), NULL);
}

// Кнопка Добавить
G_MODULE_EXPORT void on_button33_clicked (GtkButton *button, gpointer label)
{
    AddUstCabel(ID_proj);
}


/* //Отслеживание открытия окна "Данные по кабелю"
G_MODULE_EXPORT void on_wCable_show (GtkButton *button, gpointer label)

{
    activateCable=1;
} */


//Отслеживание закрытия окна "Данные по кабелю"
G_MODULE_EXPORT void on_wCable_destroy (GtkButton *button, gpointer label)

{
    activateCable=0;
}


// Кнопка Открыть
G_MODULE_EXPORT void on_button32_clicked (GtkButton *button, gpointer label)
{
	// Определяем текущий код
        GtkTreeIter iter;
        GtkTreeModel *model = gtk_tree_view_get_model (treeview_change);
        GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_change);
        gtk_tree_selection_get_selected(selection, &tm1, &iter);//Получаем итератор и модель выделенного
        path1 = gtk_tree_model_get_path ( tm1, &iter );  //Получаем путь к итератору
        if (gtk_tree_selection_get_selected (selection, NULL, &iter))
        {
            gint id_data;
            // Получение кода установленного кабеля
            gtk_tree_model_get (model, &iter,
                                C_ID_CABLE, &id_data,
                                -1);
            ShowCable(id_data);
        }

    /* if (activateCable==1)
    {
        gtk_window_present(GTK_WINDOW(wCable));
    }
    else
    {
        // Определяем текущий код
        GtkTreeIter iter;
        GtkTreeModel *model = gtk_tree_view_get_model (treeview_change);
        GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview_change);
        gtk_tree_selection_get_selected(selection, &tm1, &iter);//Получаем итератор и модель выделенного
        path1 = gtk_tree_model_get_path ( tm1, &iter );  //Получаем путь к итератору
        if (gtk_tree_selection_get_selected (selection, NULL, &iter))
        {
            gint id_data;
            // Получение кода установленного кабеля
            gtk_tree_model_get (model, &iter,
                                C_ID_CABLE, &id_data,
                                -1);
            ShowCable(id_data);
        }
    } */
}

// Кнопка Удалить
G_MODULE_EXPORT void on_button31_clicked (GtkButton *button, gpointer label)
{
	if (ShowQuestion())
    {
        DeleteUstCabel();
    }

}

// Кнопка Печать
G_MODULE_EXPORT void on_button38_clicked (GtkButton *button, gpointer label)
{
    PrintSotrudnik();
}

// Кнопка Закрыть
G_MODULE_EXPORT void on_button29_clicked (GtkButton *button, gpointer label)
{
    activateChan=0;
    SaveChange(ID_proj); // Запись значений в БД
    ReloadProjects(ID_proj); // Обновление записи в окне Проекты
    gtk_widget_destroy (wChange);
}

// Вывод таблицы
void ShowChange(int proj)
{
    ID_proj=proj;
    bChange = gtk_builder_new ();
    gtk_builder_add_from_file (bChange, "main.glade", NULL);
    wChange = GTK_WIDGET(gtk_builder_get_object(bChange, "wChange"));
    liststore_cable =  GTK_LIST_STORE(gtk_builder_get_object(bChange, "liststore_cable")); // Хранилище строк
    liststore_cable1 =  GTK_TREE_MODEL(gtk_builder_get_object(bChange, "liststore_cable"));
    treeview_change =  GTK_TREE_VIEW(gtk_builder_get_object(bChange, "treeview_change"));
    gPorNom = GTK_SPIN_BUTTON(gtk_builder_get_object(bChange, "spinbutton_PorNom"));
    gPorNomProj = GTK_SPIN_BUTTON(gtk_builder_get_object(bChange, "spinbutton_PorNomProj"));
    gObjname = GTK_ENTRY(gtk_builder_get_object(bChange, "gObjname"));
    gObjsituate = GTK_ENTRY(gtk_builder_get_object(bChange, "gObjsituate"));
    gZakazpname = GTK_ENTRY(gtk_builder_get_object(bChange, "gZakazpname"));
    gPrim = GTK_ENTRY(gtk_builder_get_object(bChange, "gPrim"));
    gSystem = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bChange, "comboboxtext1"));
    gManager = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bChange, "comboboxtext2"));
    gProjecter = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bChange, "comboboxtext3"));
    gCalendar = GTK_CALENDAR(gtk_builder_get_object(bChange, "calendar1"));
    gtk_window_set_default_size (GTK_WINDOW (wChange), 1000, 1000);
    gtk_window_set_position(GTK_WINDOW(wChange), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wChange), "Change");// Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wChange), "tables-icon.png", NULL); //Вывод иконки
    gtk_window_set_modal(GTK_WINDOW(wChange),TRUE);
    gtk_builder_connect_signals (bChange, NULL);
    LoadChange(proj);
    if(LoadUstCabel(proj))
    {
        ShowError();
    }
    gtk_widget_show_all(wChange);
}
