## arduino cron
Эта библиотека позволяет реализовать вызов пользовательских функций через определенный промежуток времени. Механизм аналогичен СRON в Linux.

### Список доступных методов
- [cron.add](#cronadd)
- [cron.handleEvents](#cronhandleevents)
- [cron.find](#cronfind)
- [cron.lastRun](#cronlastrun)
- [cron.update](#cronupdate)
- [cron.stop](#cronstop)
- [cron.isActive](#cronisactive)
- [cron.size](#cronsize)
- [cron.last](#cronlast)

### Пользовательские объекты
- [cronEvent::userFunction_t](#croneventuserfunction_t)

### Описание методов
-----------------------
#### cron.add
Добавление нового задания в планировщик
```C++
bool add(uint32_t interval, cronEvent::userFunction_t function[, bool start, const char* name])
```
* `interval` - интервал вызова пользовательской функции в ms или используйте специальные константы
* `function` - имя пользовательской функции которую необходимо вызвать (не должна принимать параметры и возвращать значение)
* `start` - (не обязательно) если установлено `true`, то пользовательская функция будет вызвана первый раз при добавлении в планировщик
* `name` - (не обязательно) читаемое имя задания по которому можно его найти и изменить

Возвращает `true` или `false`, в зависимости от успеха добавления нового задания
```C++
cron.add(cron::second, user_function_name);
```
```C++
cron.add(cron::second, [](){
  ...
});
```
```C++
if (cron.add(1000, user_function_name)) {
  Serial.println('added user function in cron');
}
```

#### cron.handleEvents
Обработчик очереди заданий
```C++
void handleEvents()
```
Добавляется в функцию loop и занимается запуском заданий
```C++
void loop() {
  cron.handleEvents();
}
```

#### cron.find
Поиск задания по имени
```C++
iterator_t cron.find(const char* name);
```
Возвращает объект задания (Class::cronEvent) или итератор end (std::list) в случае неудачи
```C++
if (cron.find("Blink")) {
  cron.update("Blink", 250);
}
```

#### cron.lastRun
Возвращает количество ms прошедших с момента последнего вызова задания.
Если параметр идентифицирующий задание не задан, то вернет значение, относящееся к последнему добавленному заданию в список.
```C++
uint32_t lastRun()
uint32_t lastRun(cronEvent &item)
uint32_t lastRun(const char* name)
uint32_t lastRun(iterator_t item)
```
Если задание не найдено, то возвращает `0`
```C++
Serial.println(cron.lastRun("Blink"));
```
```C++
iterator_t item = cron.find("Blink");
if (item) {
  uint32_t time = cron.lastRun(item);
  Serial.println(time);
}
```

#### cron.update
Обновление таймера задания.
```C++
void update(const char* name[, uint32_t interval])
```
* `name` - имя задания
* `interval` - (не обязательно) новый интервал времени для запуска задания
Обновит таймер запуска задания без вызова пользовательской функции, это отсрочит время запуска пользовательской функции на интервал времени заданный для этого задания.
Если задан не обязательный параметр `interval`, то также будет изменен интервал запуска задания.
```C++
cron.add(cron::time_1s, [](){ Serial.println("+1 blink"); }, "Blink");
cron.update("Blink", cron::time_5s);
```

#### cron.stop
Остановка выполнения задания.
```C++
void stop(const char* name)
```
После вызова устанавливает интервал запуска для задания в `0`, что приведет к полной остановке выполнения этого задания.
```C++
cron.stop("Blink");
```

#### cron.isActive
Проверяет активна задача или нет
```C++
bool isActive(const char* name)
```
Возвращает `true` в случае если задание выполняется, в противном случае вернет `false`

#### cron.size
Возвращает размер очереди заданий
```C++
size_t size()
```

#### cron.last
Возвращает последнее задание в очереди
```C++
cronEvent &last()
```

### Пользовательские объекты
-----------------------
#### cronEvent::userFunction_t
Описывает тип пользовательской функции принимаемой в качестве аргумента для метода `add`.
```C++
typedef std::function<void(void)> userFunction_t;
```
Данная конструкция описывает, что пользователь должен создать функцию, которая не принимает никакие параметры и ничего не возвращает. Ей будут доступны все глобальные объекты и переменные объявленные ранее.

### Примеры использования
-----------------------
Задание на изменение состояния светодиода
```C++
#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  pinMode(ledPin, OUTPUT);

  cron.add(cron::second, blink_f);
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}
```

Прмиер взаимодействия нескольких заданий и изменение их свойств
```C++
#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT);

  /* Main task */
  cron.add(cron::second, blink_f, "Blink");
  /* Control task */
  cron.add(cron::time_5s, control_f);
  /* Changing the state for the task "Blink" */
  cron.add(cron::time_10s, switch_f);
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

void control_f() {
  if (cron.isActive("Blink")) {
    Serial.println(F("Task \"Blink\" is active"));
  } else {
    Serial.println(F("Task \"Blink\" is not active"));
  }
}

void switch_f() {
  if (cron.isActive("Blink")) {
    cron.stop("Blink");
    Serial.println(F("The task is stopped"));
  } else {
    cron.update("Blink", cron::time_1s); // You can set any interval
    Serial.println(F("The task is resumed"));
  }
}
```
