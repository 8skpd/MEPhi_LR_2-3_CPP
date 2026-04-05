#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/entry.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>


#include "Deque.h" 
#include "Exceptions.h"

#include <sstream>
#include <string>
#include <stdexcept>

class DequeGUI : public Gtk::Window {
public:
    DequeGUI() 
    : m_TitleLabel("Visual Deque Monitor (Lab 3)"),
      m_StatusLabel("Status: Ready"),
      m_PushBackBtn("Push Back"),
      m_PushFrontBtn("Push Front"),
      m_PopBackBtn("Pop Back"),
      m_PopFrontBtn("Pop Front"),
      m_SortBtn("Sort"),
      m_ClearBtn("Clear"),
      m_InversionsBtn("Count Inversions")
    {
        // Настройки окна
        set_title("C++ Deque Visualizer");
        set_border_width(15);
        set_default_size(800, 600);

        // Инициализация модели таблицы (список строк)
        // Колонки: Индекс, Значение
        m_refTreeModel = Gtk::ListStore::create(m_Columns);
        m_TreeView.set_model(m_refTreeModel);

        // Настройка внешнего вида таблицы
        m_TreeView.set_rules_hint(true); // Чередование цветов строк
        m_TreeView.append_column("Index", m_Columns.m_col_id);
        m_TreeView.append_column("Value", m_Columns.m_col_value);
        
        // Выравнивание колонок
        auto col_id = m_TreeView.get_column(0);
        if(col_id) col_id->set_alignment(0.5); // По центру
        auto col_val = m_TreeView.get_column(1);
        if(col_val) col_val->set_alignment(0.5);

        // Скролл для таблицы (если элементов много)
        m_ScrollWin.add(m_TreeView);
        m_ScrollWin.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        m_ScrollWin.set_shadow_type(Gtk::SHADOW_IN);

        // Поле ввода
        m_Entry.set_placeholder_text("Enter integer value...");
        m_Entry.set_width_chars(20);
        m_Entry.signal_activate().connect(sigc::mem_fun(*this, &DequeGUI::on_push_back)); // Enter работает как Push Back

        // Группировка кнопок управления
        m_ControlBox.pack_start(m_Entry, Gtk::PACK_SHRINK);
        m_ControlBox.pack_start(m_PushBackBtn, Gtk::PACK_SHRINK);
        m_ControlBox.pack_start(m_PushFrontBtn, Gtk::PACK_SHRINK);
        m_ControlBox.set_spacing(10);

        m_OpBox.pack_start(m_PopFrontBtn, Gtk::PACK_EXPAND_WIDGET);
        m_OpBox.pack_start(m_PopBackBtn, Gtk::PACK_EXPAND_WIDGET);
        m_OpBox.pack_start(m_SortBtn, Gtk::PACK_EXPAND_WIDGET);
        m_OpBox.pack_start(m_InversionsBtn, Gtk::PACK_EXPAND_WIDGET);
        m_OpBox.pack_start(m_ClearBtn, Gtk::PACK_EXPAND_WIDGET);
        m_OpBox.set_spacing(10);

        // Общий вертикальный
        m_VBox.pack_start(m_TitleLabel, Gtk::PACK_SHRINK);
        m_VBox.pack_start(m_ControlBox, Gtk::PACK_SHRINK);
        m_VBox.pack_start(m_OpBox, Gtk::PACK_SHRINK);
        m_VBox.pack_start(m_ScrollWin, Gtk::PACK_EXPAND_WIDGET); // Таблица растягивается
        m_VBox.pack_start(m_StatusLabel, Gtk::PACK_SHRINK);
        
        m_VBox.set_spacing(15);
        add(m_VBox);

        // Подключение сигналов (событий клика) к функциям
        m_PushBackBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_push_back));
        m_PushFrontBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_push_front));
        m_PopBackBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_pop_back));
        m_PopFrontBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_pop_front));
        m_SortBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_sort));
        m_ClearBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_clear));
        m_InversionsBtn.signal_clicked().connect(sigc::mem_fun(*this, &DequeGUI::on_count_inversions));

        // Первоначальная отрисовка (пустая)
        refresh_table();
        
        show_all_children();
    }

protected:
    void on_push_back() {
        try {
            int val = parse_entry();
            m_Deque.PushBack(val);
            set_status("Pushed " + std::to_string(val) + " to Back");
            refresh_table();
            m_Entry.set_text(""); // Очистить поле после ввода
            m_Entry.grab_focus(); // Вернуть фокус в поле
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_push_front() {
        try {
            int val = parse_entry();
            m_Deque.PushFront(val);
            set_status("Pushed " + std::to_string(val) + " to Front");
            refresh_table();
            m_Entry.set_text("");
            m_Entry.grab_focus();
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_pop_back() {
        try {
            if (m_Deque.IsEmpty()) throw std::runtime_error("Deque is empty!");
            int val = m_Deque.PopBack();
            set_status("Popped " + std::to_string(val) + " from Back");
            refresh_table();
        } catch (const EmptyStructureException&) {
            show_error("Cannot pop: Deque is empty.");
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_pop_front() {
        try {
            if (m_Deque.IsEmpty()) throw std::runtime_error("Deque is empty!");
            int val = m_Deque.PopFront();
            set_status("Popped " + std::to_string(val) + " from Front");
            refresh_table();
        } catch (const EmptyStructureException&) {
            show_error("Cannot pop: Deque is empty.");
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_sort() {
        try {
            if (m_Deque.IsEmpty()) {
                set_status("Nothing to sort (Empty)");
                return;
            }
            m_Deque.Sort();
            set_status("Sorted successfully");
            refresh_table();
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_count_inversions() {
        try {
            int count = m_Deque.CountInversions();
            std::stringstream ss;
            ss << "Inversions count: " << count;
            set_status(ss.str());

            Gtk::MessageDialog dialog(*this, ss.str(), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
            dialog.run();
        } catch (const std::exception& e) {
            show_error(e.what());
        }
    }

    void on_clear() {
        m_Deque = Deque<int>(); 
        set_status("Deque cleared");
        refresh_table();
    }


    // Парсинг числа из поля ввода с проверкой
    int parse_entry() {
        Glib::ustring text = m_Entry.get_text();
        if (text.empty()) {
            throw std::runtime_error("Please enter a number first.");
        }
        
        try {
            size_t pos;
            int val = std::stoi(text, &pos);
            if (pos != text.length()) {
                throw std::runtime_error("Invalid characters in input.");
            }
            return val;
        } catch (...) {
            throw std::runtime_error("Input is not a valid integer.");
        }
    }

    // Синхронизация дека и таблицы
    void refresh_table() {
        m_refTreeModel->clear(); // Полная очистка таблицы

        int len = m_Deque.GetLength();
        for (int i = 0; i < len; ++i) {
            // Добавляем новую строку
            Gtk::TreeModel::Row row = *(m_refTreeModel->append());
            
            // Заполняем ячейки
            row[m_Columns.m_col_id] = i;
            row[m_Columns.m_col_value] = m_Deque[i]; 
        }

        // Обновляем заголовок окна с текущим размером
        std::stringstream title;
        title << "C++ Deque Visualizer (Size: " << len << ")";
        set_title(title.str());
    }

    void set_status(const std::string& msg) {
        m_StatusLabel.set_text("Status: " + msg);
    }

    void show_error(const std::string& msg) {
        Gtk::MessageDialog dialog(*this, msg, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
        dialog.run();
        set_status("Error occurred");
    }

    Deque<int> m_Deque;

    // Виджеты
    Gtk::Box m_VBox{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box m_ControlBox{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Box m_OpBox{Gtk::ORIENTATION_HORIZONTAL};
    
    Gtk::Label m_TitleLabel;
    Gtk::Label m_StatusLabel;
    
    Gtk::Entry m_Entry;
    
    Gtk::Button m_PushBackBtn;
    Gtk::Button m_PushFrontBtn;
    Gtk::Button m_PopBackBtn;
    Gtk::Button m_PopFrontBtn;
    Gtk::Button m_SortBtn;
    Gtk::Button m_ClearBtn;
    Gtk::Button m_InversionsBtn;

    Gtk::ScrolledWindow m_ScrollWin;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

    // Описание колонок таблицы
    class Columns : public Gtk::TreeModelColumnRecord {
    public:
        Columns() { add(m_col_id); add(m_col_value); }
        Gtk::TreeModelColumn<int> m_col_id;
        Gtk::TreeModelColumn<int> m_col_value;
    } m_Columns;
};

int main(int argc, char *argv[]) {
    // Инициализация GTK
    auto app = Gtk::Application::create(argc, argv, "org.example.cpplab3.deque");

    // Создание окна
    DequeGUI window;

    // Запуск цикла событий
    return app->run(window);
}