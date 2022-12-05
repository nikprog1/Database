int CurSotrudnik;
int CurSotrudnikPrint;
int CurOtdel;
char CurFoto[256];
char CurDate[256];
guint gDay, gMonth, gYear;

// Обозначения полей таблицы Сотрудники
enum
{
    C_ID, C_NAME, C_OKLAD, C_KOD_OTDELA, C_FOTO, C_DAY, C_MONTH, C_YEAR
};

// Обозначения полей таблицы Отделы
enum
{
    C_ID_OTDELA, C_OTDEL, C_BOSS
};


// Вывод карточки сотрудника на печать
static void DrawProject(GtkPrintOperation *op, GtkPrintContext *context, int page_nr)
{
    cairo_t *cr;
    gchar buf[1000];
    cr = gtk_print_context_get_cairo_context (context);


    char name_data[1000];
    int oklad_data;
    char name_otdel[1000];

    strcpy(name_data, gtk_entry_get_text(GTK_ENTRY(gSotr)));
    oklad_data = gtk_spin_button_get_value_as_int (gOklad);
    // Получение имени отдела
    strcpy(name_otdel,gtk_combo_box_text_get_active_text(gOtdel));
    gtk_calendar_get_date(gCalendar, &gYear, &gMonth, &gDay);
/* 
    // Выводим заголовок
    cairo_move_to (cr, 80.0, 10.0);
    cairo_set_font_size (cr, 5.0);
    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text (cr, "Карточка сотрудника");
    cairo_stroke(cr);

    // Выводим поле "Фамилия"
    cairo_select_font_face(cr, "sans serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to (cr, 10.0, 20.0);
    cairo_set_font_size (cr, 5.0);
    sprintf(buf, "Фамилия - %s", name_data);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);

    // Выводим поле "Оклад"
    cairo_move_to (cr, 10.0, 25.0);
    sprintf(buf, "Оклад - %d руб.", oklad_data);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);

    // Выводим поле "Отдел"
    cairo_move_to (cr, 10.0, 30.0);
    sprintf(buf, "Отдел - %s", name_otdel);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);

    // Выводим поле "Дата рождения"
    cairo_move_to (cr, 10.0, 35.0);
    sprintf(buf, "Дата рождения - %02d.%02d.%4d", gDay, ++gMonth, gYear);
    cairo_show_text (cr, buf);
    cairo_stroke(cr);

    // Выводим поле "Фото"
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(CurFoto, NULL);
    // Масштабируем фото
    cairo_save(cr);                 // сохраняем контекст рисования
    cairo_scale (cr, 0.5, 0.5);     // масштабируем контекст рисования
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 20, 80);
    cairo_paint (cr);               // выводим изображение
    cairo_restore(cr);              // восстанавливаем контекст рисования */

    g_object_unref (pixbuf);
}


// Печать карточки сотрудника
void PrintSotrudnik()
{
    GtkPrintOperation *op;

    op = gtk_print_operation_new ();

    gtk_print_operation_set_n_pages (op, 1);
    gtk_print_operation_set_unit (op, GTK_UNIT_MM);
    g_signal_connect (op, "draw_page", G_CALLBACK (DrawPageSotrudnik), NULL);
    gtk_print_operation_run (op, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (w_sotrudnik), NULL);
}
