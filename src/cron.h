#ifndef cron_h
#define cron_h

#include <list>

namespace {
  class cronEvent {
    public:
      typedef std::function<void(void)> userFunction_t;
      
      cronEvent(uint32_t interval, userFunction_t function, const char* name) {
        this->interval = interval;
        this->function = function;
        this->_name = name;
      }
    
      const char* const name() {
        return this->_name;
      }

      void run() {
        this->function();
        this->time = millis();
      }
      
      uint32_t interval = 0;
      uint32_t time = 0;
    private:
      const char* _name;
      userFunction_t function;
  };

  class cron {
    public:
      typedef std::list<cronEvent> list_t;
      typedef list_t::iterator iterator_t;
      /* 
        Читаемые имена для самых часто используемых интервалов времени. 
      */
      enum {
        time_1s  = 1000,
        time_5s  = 5000,
        time_10s = 10000,
        time_15s = 15000,
        time_30s = 30000,
        time_1m  = 60000,
        time_5m  = 300000,
        time_10m = 600000,
        time_15m = 900000,
        time_30m = 1800000,
        time_1h  = 3600000,
        time_5h  = 18000000,
        time_10h = 36000000,
        time_12h = 43200000,
        time_1d  = 86400000,
      };
      /* 
        Cокращения для фундаментальных значений.
      */
      enum {
        second = cron::time_1s,
        minute = cron::time_1m,
        hour   = cron::time_1h,
        day    = cron::time_1d
      };
      /*
        Добавление нового задания в планировщик.
        В качестве аргумента принимает интервал вызова в ms и имя вызываемой функции.
        Последний, не обязательный, параметр устанавливает идентификатор для поиска задачи.
        Параметр start при значении true заставит произвести первый запуск при добавлении задачи в очередь, а не по таймеру.
      */
      bool add(uint32_t interval, cronEvent::userFunction_t function, bool start, const char* name = 0) {
        if (!this->add(interval, function, name)) return false;
        if (start) function();
        return true;
      }
      bool add(uint32_t interval, cronEvent::userFunction_t function, const char* name = 0) {
        if (name) {
          if (this->find(name) != this->list.end()) return false;
        }
        cronEvent event(interval, function, name);
        this->list.push_back(event);
        return true;
      }
      /*
        Обработчик очереди заданий.
        Прописывается в основном цикле программы как cron.handleEvents();
      */  
      void handleEvents() {
        for(auto &item: list) {
          if (item.interval == 0) continue;
          if (this->lastRun(item) > item.interval) item.run();
        }
      }
      /*
        Поиск задания по имени.
      */
      iterator_t find(const char* name) {
        for (iterator_t item = this->list.begin(); item != this->list.end(); item++) {
          if (item->name()) {
            if (!strcmp(item->name(), name)) return item;
          }
        }
        return this->list.end();
      }
      /*
        Возвращает количество ms прошедших с момента последнего вызова задания.
      */
      uint32_t lastRun() { return this->lastRun(this->last()); }
      uint32_t lastRun(cronEvent &item)  { return millis() - item.time; }
      uint32_t lastRun(const char* name) { return this->lastRun(this->find(name)); }
      uint32_t lastRun(iterator_t item)  { return item != this->list.end() ? millis() - item->time : 0; }
      /*
        Обновление таймера задания.
        Можно задать новый интервал времени.
      */
      void update(const char* name) {
        iterator_t item = this->find(name);
        if (item != this->list.end()) item->time = millis();
      }
      void update(const char* name, uint32_t interval) {
        iterator_t item = this->find(name);
        if (item != this->list.end()) {
          item->interval = interval;
          item->time = millis();
        }
      }
      /*
        Остановка выполнения задания.
      */
      void stop(const char* name) {
        iterator_t item = this->find(name);
        if (item != this->list.end()) item->interval = 0;
      }
      /*
        Проверяет активна задача или нет.
      */
      bool isActive(const char* name) {
        iterator_t item = this->find(name);
        if (item == this->list.end()) return false;
        else return item->interval;
      }

      size_t size() { return this->list.size(); }
      cronEvent &last() { return this->list.back(); }
    private:
      list_t list;
  } cron;
}

#endif