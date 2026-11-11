#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

/*
==========================================================================================================================================
                        ДОМАШНЄ ЗАВДАННЯ 34 тема: абстрактний клас, чисто віртуальні методи, інтерфейси.
==========================================================================================================================================
*/

// ==================== РІВЕНЬ 3. Інтерфейс Damageable ====================
// Оголошений першим, бо його використовує Hero::attack

class Damageable
{
public:
    virtual void takeDamage(int damage) = 0;
    virtual bool isDestroyed() const = 0;
    virtual ~Damageable() = default;
};

// ==================== Hero ====================

class Hero
{
private:
    string name;
    int hp;
    int maxHp;
    int gold;
    int keys;
    int power;                            // рівень 3

public:
    Hero(const string& name, int hp, int maxHp, int power)
        : name(name), hp(hp), maxHp(maxHp), gold(0), keys(0), power(power)
    {}

    string getName() const
    {
        return name;
    }

    void addGold(int amount)
    {
        gold += amount;
    }

    bool spendGold(int amount)
    {
        if (amount > gold)
        {
            return false;
        }
        gold -= amount;
        return true;
    }

    void addKey()
    {
        keys++;
    }

    bool useKey()
    {
        if (keys == 0)
        {
            return false;
        }
        keys--;
        return true;
    }

    void heal(int amount)
    {
        hp = (hp + amount > maxHp) ? maxHp : hp + amount;
    }

    void print() const
    {
        cout << "  " << name << ": hp " << hp << "/" << maxHp << ", золото " << gold << ", ключів " << keys << endl;
    }

    // Рівень 3: герой б'є будь-що, що можна пошкодити
    void attack(Damageable& target) const
    {
        cout << name << " б'є: " << power << " шкоди" << endl;
        target.takeDamage(power);
    }
};

// ==================== РІВЕНЬ 1. Інтерфейс Interactable ====================

// Інтерфейс: жодних полів, лише «що вміє»
class Interactable
{
public:
    virtual string describe() const = 0;
    virtual void interact(Hero& hero) = 0;
    /*
         6. (Рівень 2) Навіщо інтерфейсу Interactable віртуальний деструктор, якщо в нього немає жодного поля?
            Де в програмі об'єкт видаляється через покажчик на інтерфейс?
    */
    virtual ~Interactable() = default;
};

/*
    2. Чому Door, Chest і Fountain не мають спільного базового класу з полями (наприклад, DungeonObject з name і hp)?
       Чим інтерфейс тут кращий?
    7. (Рівень 3) Door реалізує два інтерфейси. Які методи доступні через asObject, а які — через asTarget?
       Чому множинне успадкування від інтерфейсів не має проблеми ромба, яку ми бачили в темі про успадкування?
*/
class Door : public Interactable, public Damageable
{
private:
    string name;
    bool locked;
    int durability;

public:
    Door(const string& name, bool locked, int durability) : name(name), locked(locked), durability(durability)
    {}

    // ----- Interactable -----
    string describe() const override
    {
        return name + (locked ? " (зачинені)" : " (відчинені)");
    }

    void interact(Hero& hero) override
    {
        if (!locked)
        {
            cout << "Ви проходите далі" << endl;
        }
        else if (hero.useKey())
        {
            locked = false;
            cout << "Ключ підійшов! " << name << " відчинено" << endl;
        }
        else
        {
            cout << "Зачинено. Потрібен ключ" << endl;
        }
    }

    // ----- Damageable -----
    void takeDamage(int damage) override
    {
        if (durability == 0)
        {
            return;
        }
        durability = (damage >= durability) ? 0 : durability - damage;
        if (durability == 0)
        {
            locked = false;
            cout << name << " виламано!" << endl;
        }
    }

    bool isDestroyed() const override
    {
        return durability == 0;
    }
};

class Chest : public Interactable
{
private:
    int gold;
    bool hasKeyInside;
    bool opened;

public:
    Chest(int gold, bool hasKeyInside) : gold(gold), hasKeyInside(hasKeyInside), opened(false)
    {}

    string describe() const override
    {
        return opened ? "Скриня (порожня)" : "Скриня";
    }

    void interact(Hero& hero) override
    {
        if (opened)
        {
            cout << "Скриня порожня" << endl;
            return;
        }
        opened = true;
        hero.addGold(gold);
        cout << "Знайдено " << gold << " золота";
        if (hasKeyInside)
        {
            hero.addKey();
            cout << " і ключ";
        }
        cout << endl;
    }
};

class Fountain : public Interactable
{
private:
    int healAmount;

public:
    Fountain(int healAmount) : healAmount(healAmount)
    {}

    string describe() const override
    {
        return "Фонтан зцілення";
    }

    void interact(Hero& hero) override
    {
        hero.heal(healAmount);
        cout << "Ви п'єте з фонтану: +" << healAmount << " hp" << endl;
    }
};

/*
     1. Функція use приймає Interactable&. Чому вона працює і з дверима, і зі скринею, і з фонтаном, хоча ці класи
        між собою ніяк не пов'язані?
     5. (Рівень 2) Скільки рядків у функції use довелося змінити, коли з'явились торговець і цілителька? Чому?
*/
void use(Hero& hero, Interactable& object)
{
    cout << "> " << object.describe() << endl;
    object.interact(hero);
    hero.print();
}

// ==================== РІВЕНЬ 2. Абстрактний клас Npc ====================

// 3. (Рівень 2) Чим абстрактний клас Npc відрізняється від інтерфейсу Interactable? Чому в Npc є поля і готові методи?
// 4. (Рівень 2) Чому не можна створити об'єкт Npc? Що станеться, якщо в Healer забути перевизначити interact?
// Абстрактний: має поля і готові методи, але interact залишається чисто віртуальним
class Npc : public Interactable
{
protected:
    string name;
    string phrase;

public:
    Npc(const string& name, const string& phrase) : name(name), phrase(phrase)
    {}

    string describe() const override     // спільний для всіх NPC
    {
        return name;
    }

    void greet() const
    {
        cout << name << ": \"" << phrase << "\"" << endl;
    }
};

class Merchant : public Npc
{
private:
    int potionPrice;
    int potionHeal;

public:
    Merchant(const string& name, int potionPrice, int potionHeal)
        : Npc(name, "Зілля! Свіже зілля!"), potionPrice(potionPrice), potionHeal(potionHeal)
    {}

    void interact(Hero& hero) override
    {
        greet();
        if (hero.spendGold(potionPrice))
        {
            hero.heal(potionHeal);
            cout << "Ви купили зілля: +" << potionHeal << " hp" << endl;
        }
        else
        {
            cout << "Не вистачає золота" << endl;
        }
    }
};

class Healer : public Npc
{
private:
    bool helped;

public:
    Healer(const string& name) : Npc(name, "Хай світло оберігає тебе"), helped(false)
    {}

    void interact(Hero& hero) override
    {
        greet();
        if (helped)
        {
            cout << "Я вже допомогла тобі сьогодні" << endl;
            return;
        }
        helped = true;
        hero.heal(50);
        cout << "Цілителька лікує вас: +50 hp" << endl;
    }
};

// ==================== РІВЕНЬ 3. Павутина ====================

// Павутину можна лише розрубати — взаємодіяти з нею не можна, тому Interactable вона не реалізує
class Cobweb : public Damageable
{
private:
    int durability;

public:
    Cobweb(int durability) : durability(durability)
    {}

    void takeDamage(int damage) override
    {
        if (durability == 0)
        {
            return;
        }
        durability = (damage >= durability) ? 0 : durability - damage;
        if (durability == 0)
        {
            cout << "Павутину розрубано" << endl;
        }
    }

    bool isDestroyed() const override
    {
        return durability == 0;
    }
};

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "===== РІВЕНЬ 1 =====" << endl;
    Hero conan("Конан", 60, 100, 10);
    Door oakDoor("Дубові двері", true, 50);
    Chest chest(30, true);
    Fountain fountain(25);

    use(conan, oakDoor);                  // зачинено — ключа ще немає
    use(conan, chest);                    // золото і ключ
    use(conan, oakDoor);                  // ключ підійшов
    use(conan, chest);                    // порожня
    use(conan, fountain);                 // 60 → 85

    cout << endl << "===== РІВЕНЬ 2 =====" << endl;
    // Npc npc("Хтось", "Привіт");        // ПОМИЛКА: Npc абстрактний — interact не має реалізації

    Hero lira("Ліра", 30, 100, 10);
    int count = 5;
    Interactable** room = new Interactable * [count];
    room[0] = new Chest(20, false);
    room[1] = new Merchant("Гном Борін", 15, 40);
    room[2] = new Healer("Сестра Мирна");
    room[3] = new Merchant("Гном Борін", 15, 40);
    room[4] = new Fountain(10);

    for (int i = 0; i < count; i++)
    {
        use(lira, *room[i]);              // use не знає, хто перед нею: скриня, NPC чи фонтан
    }

    for (int i = 0; i < count; i++)
    {
        delete room[i];                   // через покажчик на інтерфейс — потрібен віртуальний деструктор
    }
    delete[] room;

    cout << endl << "===== РІВЕНЬ 3 =====" << endl;
    Door ironDoor("Залізні двері", true, 30);
    Cobweb cobweb(15);

    use(conan, ironDoor);                 // ключа немає — зачинено
    while (!ironDoor.isDestroyed())
    {
        conan.attack(ironDoor);           // Door& → Damageable&
    }
    use(conan, ironDoor);                 // тепер відчинені

    while (!cobweb.isDestroyed())
    {
        conan.attack(cobweb);             // Hero::attack не знає ні про двері, ні про павутину
    }

    // Той самий об'єкт — два «обличчя»
    Interactable* asObject = &ironDoor;
    Damageable* asTarget = &ironDoor;
    cout << "Через Interactable: " << asObject->describe() << endl;
    cout << "Через Damageable: " << (asTarget->isDestroyed() ? "зламані" : "цілі") << endl;
    // asObject->isDestroyed();           // ПОМИЛКА: в Interactable такого методу немає
    // asTarget->describe();              // ПОМИЛКА: в Damageable такого методу немає

    return 0;
}

/*
  
  1. Функція use приймає Interactable&. Чому вона працює і з дверима, і зі скринею, і з фонтаном, хоча ці класи
     між собою ніяк не пов'язані?
  2. Чому Door, Chest і Fountain не мають спільного базового класу з полями (наприклад, DungeonObject з name і hp)?
     Чим інтерфейс тут кращий?
  3. (Рівень 2) Чим абстрактний клас Npc відрізняється від інтерфейсу Interactable? Чому в Npc є поля і готові методи?
  4. (Рівень 2) Чому не можна створити об'єкт Npc? Що станеться, якщо в Healer забути перевизначити interact?
  5. (Рівень 2) Скільки рядків у функції use довелося змінити, коли з'явились торговець і цілителька? Чому?
  6. (Рівень 2) Навіщо інтерфейсу Interactable віртуальний деструктор, якщо в нього немає жодного поля?
     Де в програмі об'єкт видаляється через покажчик на інтерфейс?
  7. (Рівень 3) Door реалізує два інтерфейси. Які методи доступні через asObject, а які — через asTarget?
     Чому множинне успадкування від інтерфейсів не має проблеми ромба, яку ми бачили в темі про успадкування?

  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ» (еталон для розбору)

  1. 

  2. 

  3. 

  4. 

  5. 

  6. 

  7. 

*/
