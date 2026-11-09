#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

/*
==========================================================================================================================================
                                                                Інтерфейс
==========================================================================================================================================

  ІНТЕРФЕЙС — абстрактний клас без полів, у якому всі методи чисто віртуальні (плюс віртуальний деструктор). 
  Він описує, ЩО об'єкт уміє, і нічого не каже про те, ЯК.
  
  Клас РЕАЛІЗУЄ інтерфейс, коли успадковує його і перевизначає всі його методи. Таким чином інтерфейс стає для класу контрактом
  про те, що клас зобов'язується вміти робити, не уточнюючи, як саме він це робитиме.

                                                    Основні переваги використання інтерфейсів:
  - Інтерфейс дозволяє однаково працювати з класами, НЕ пов'язаними відношенням «є»: лампа, термостат і чайник —
    різні пристрої, але всі їх можна вмикати і вимикати.
  - Код, написаний через інтерфейс, працює з будь-яким класом, що його реалізує; якщо з'явиться новий клас,
    цей код змінювати не доведеться.
  - Клас може реалізовувати скільки завгодно інтерфейсів — це звичайне використання множинного успадкування. При цьому дублювання полів,
    як у проблемі ромба, не виникає: в інтерфейсах полів немає.
  - Загалом це дає можливість гнучко розширювати клас, не боячись, що архітектура не витримає: потрібні нові можливості — реалізуємо
    новий інтерфейс.

*/

// Інтерфейс: «можна вмикати і вимикати»
class ISwitchable
{
public:
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual ~ISwitchable() = default;
};

// Інтерфейс: «можна регулювати яскравість»
class IDimmable
{
public:
    virtual void setBrightness(int percent) = 0;
    virtual ~IDimmable() = default;
};

// Інтерфейс: «щось вимірює»
class IMeasurable
{
public:
    virtual string getReading() const = 0;
    virtual ~IMeasurable() = default;
};

// Розумна лампа: два інтерфейси — вмикається і регулює яскравість
class SmartLamp : public ISwitchable, public IDimmable
{
private:
    string name;
    int brightness;

public:
    SmartLamp(const string& name) : name(name), brightness(100)
    {}

    void turnOn() override
    {
        cout << name << ": увімкнено, яскравість " << brightness << "%" << endl;
    }

    void turnOff() override
    {
        cout << name << ": вимкнено" << endl;
    }

    void setBrightness(int percent) override
    {
        brightness = (percent < 0) ? 0 : (percent > 100 ? 100 : percent);
        cout << name << ": яскравість " << brightness << "%" << endl;
    }
};

// Термостат: теж два інтерфейси, але інші — вмикається і вимірює температуру
class Thermostat : public ISwitchable, public IMeasurable
{
private:
    string name;
    int temperature;

public:
    Thermostat(const string& name, int temperature) : name(name), temperature(temperature)
    {}

    void turnOn() override
    {
        cout << name << ": опалення увімкнено" << endl;
    }

    void turnOff() override
    {
        cout << name << ": опалення вимкнено" << endl;
    }

    string getReading() const override
    {
        return name + ": " + to_string(temperature) + " °C";
    }
};

// Чайник: лише один інтерфейс — його можна тільки вмикати і вимикати
class Kettle : public ISwitchable
{
private:
    string name;

public:
    Kettle(const string& name) : name(name)
    {}

    void turnOn() override
    {
        cout << name << ": закипає" << endl;
    }

    void turnOff() override
    {
        cout << name << ": вимкнено" << endl;
    }
};

// Сценарій працює з будь-яким пристроєм, який можна вимкнути, — не знає, лампа це чи чайник
void goodNight(ISwitchable* devices[], int count)
{
    cout << "Сценарій «На добраніч»:" << endl;
    for (int i = 0; i < count; i++)
    {
        devices[i]->turnOff();
    }
}

// Вечірнє світло — лише для того, що вміє регулювати яскравість
void setEveningLight(IDimmable& light)
{
    light.setBrightness(30);
}

void showReading(const IMeasurable& sensor)
{
    cout << sensor.getReading() << endl;
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // ISwitchable device;                // ПОМИЛКА: інтерфейс абстрактний

    SmartLamp lamp("Лампа у вітальні");
    Thermostat thermostat("Термостат", 21);
    Kettle kettle("Чайник");

    cout << "1. Різні пристрої через один інтерфейс ISwitchable" << endl;
    const int deviceCount = 3;
    ISwitchable* devices[deviceCount] = { &lamp, &thermostat, &kettle };
    for (int i = 0; i < deviceCount; i++)
    {
        devices[i]->turnOn();
    }

    cout << endl << "2. Додаткові можливості — лише в тих, хто реалізує відповідний інтерфейс" << endl;
    setEveningLight(lamp);
    showReading(thermostat);
    // setEveningLight(kettle);           // ПОМИЛКА: чайник не IDimmable
    // showReading(lamp);                 // ПОМИЛКА: лампа не IMeasurable

    cout << endl << "3. Один об'єкт — кілька інтерфейсів" << endl;
    ISwitchable* asSwitch = &lamp;        // через ISwitchable видно лише turnOn і turnOff
    IDimmable* asDimmer = &lamp;          // через IDimmable — лише setBrightness
    asDimmer->setBrightness(70);
    asSwitch->turnOn();
    // asSwitch->setBrightness(50);       // ПОМИЛКА: в ISwitchable такого методу немає

    cout << endl;
    goodNight(devices, deviceCount);

    return 0;
}

/*
==========================================================================================================================================
                                                  ІНСАЙТИ / ЛАЙФХАКИ / BEST PRACTICES
==========================================================================================================================================

  1. Програмуйте на рівні інтерфейсу, а не реалізації. Додайте новий пристрій, що реалізує ISwitchable (наприклад, розумну
     розетку), — сценарій goodNight запрацює з ним без жодної зміни.

  2. Де це в реальних програмах. Розумний дім так і влаштований: у стандарті Matter і в Google Home пристрій описується
     набором можливостей (увімкнення, яскравість, температура), а не тим, чим він є. В ігрових рушіях так само поширені
     IDamageable («можна пошкодити») та IInteractable («можна взаємодіяти»). Інтерфейс також — договір між програмістами:
     один пише сценарії, що працюють з ISwitchable, інший паралельно пише нові пристрої, і їм досить домовитися про інтерфейс.

  3. Префікс I (ISwitchable) — не стандарт C++: стандартна бібліотека його не використовує, і в кожному проєкті своя
     домовленість. Але в C# це обов'язкова конвенція, як і в Unreal Engine. У Java і C# є ключове слово interface,
     і клас може успадкувати лише один клас, але реалізувати багато інтерфейсів.

  4. Інтерфейсу завжди потрібен віртуальний деструктор (= default): з об'єктами часто працюють саме через покажчик
     на інтерфейс і видаляють через нього.

  5. Не робіть інтерфейс «про запас» для класу, у якого лише одна реалізація і інших не передбачається, — це зайва
     складність. Інтерфейс потрібен там, де є кілька різних класів, з якими треба працювати однаково.

  6. Поле name повторюється в усіх трьох пристроях. Якщо спільного стає багато, його виносять в абстрактний клас
     (наприклад, SmartDevice з полем name), а інтерфейси додають можливості: class SmartLamp : public SmartDevice, public IDimmable.

==========================================================================================================================================
                                                              ПРАКТИКА
==========================================================================================================================================

  Завдання 1. Розумна розетка.
  Додайте до прикладу клас SmartPlug : public ISwitchable, public IMeasurable:
    - поля string name; int watts (споживання, Вт);
    - конструктор SmartPlug(const string& name, int watts);
    - turnOn() / turnOff() — виводять «<name>: живлення увімкнено / вимкнено»;
    - getReading() — «<name>: <watts> Вт».
  У main:
    - додайте розетку в масив devices і перевірте, що цикл увімкнення і goodNight працюють без змін;
    - виведіть її показник через showReading.

  Завдання 2. Офісна техніка.
    - Інтерфейси (кожен з віртуальним деструктором = default):
        IPrintable — virtual void print(const string& document) = 0;
        IScannable — virtual string scan() = 0;
        ICopyable  — virtual void copy(int copies) = 0.
    - Printer : public IPrintable — print виводить «Принтер друкує: <document>».
    - Scanner : public IScannable — scan виводить «Сканер сканує...» і повертає рядок «скан документа».
    - MultifunctionDevice (БФП) — реалізує всі три інтерфейси: print і scan — як у принтера і сканера,
      copy(int copies) — виводить «БФП робить копії: <copies>».
    - Функція void printReport(IPrintable& device) — друкує документ «Звіт за листопад».
  У main:
    - надрукуйте звіт на принтері і на БФП через printReport;
    - відскануйте документ сканером і БФП, зробіть 3 копії на БФП;
    - переконайтеся, що printReport(scanner) не компілюється, і поясніть у коментарі чому.
*/
