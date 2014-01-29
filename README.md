# spj

Учебный парсер JSON на языке C, создаваемый на основе обсуждения, открытого на сайте [ХэшКод.Исследования](http://hashcode.ru/research/): [Простейший парсер JSON на C/Objective-C](http://hashcode.ru/research/291943/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%B5%D0%B9%D1%88%D0%B8%D0%B9-%D0%BF%D0%B0%D1%80%D1%81%D0%B5%D1%80-json-%D0%BD%D0%B0-c-objective-c).  

## Установка

```bash
git clone git@github.com:stanislaw/spj.git
cd spj/
make # компиляция и запуск
```

## Первичные концепция, ТЗ и Черновик API в одном разделе

* Скорость разбора.
* Скорость доступа: функция доступа(чтения) элементов на любой глубине (в том числе по ключам(name) объектов Object).

* Результат парсинга - набор "read-only" структур для многократного поиска данных.

* Исходные данные копируются 

> Плюсом является то, что после окончания разбора память, занимаемую исходной json строкой можно освободить.

> Еще один плюс такого решения (это уже дальнейшее развитие), что в
некоторых случаях можно читать данные поблочно (из файла или сокета),
например используя технику, которую продемонстрировал @VladD.

> Не похоже, что в процессе разбора придется обращаться к ранее
прочитанным данным, более чем на одну позицию.

> ...для ускорения разбора я бы предложил (не для первой версии,
конечно) заранее аллоцировать "арену" — большой кусок памяти, в котором
и располагать данные JSON-объекта. Дело в том, что malloc (и new) —
сравнительно медленные операции, требующие heap lock для
многопоточности.

* О структурах Object и Array. Вы хотели бы сделать их обе на динамических массивах, или интересуетесь также программированием списков? В принципе, если структуры после парсинга не будут модифицироваться, то на массивах будет эффективней.

> На массивах.

* А поиск по имени последовательный (типичный размер не очень большой).

> ...Если вы хотите реально быстрый доступ, вам придётся в JSONObject
реализовать hashtable или rb tree (в этом вопросе засветилось множество
имплементаций). Потому что для большого количества элементов в
JSONObject'е линейный поиск слишком медленный. С другой стороны, для
маленького количества элементов линейный поиск быстрее. Если ваш
JSONObject есть read-only, вы можете при его создании подсчитать
количество ключей и выбрать ту или иную стратегию хранения. Для ещё
большей оптимизации поиска вы можете построить по ключам поисковое
дерево.

> Выбор стратегии хранения можно сделать простым: количество ключей
больше N => храним в hashtable, меньше/равно => в массиве. Оптимальное N
можно подобрать, подсчитав время пробега на разнотипных данных с
различными N.

* Красивая задача - это чтобы можно было удобно работать с spj без привлечения Cocoa API.

* Есть альтернативный вариант концепции - или можно его сделать конфигурируемым - это __вообще не хранить/не создавать никакие структуры для хранения__, а прямо на лету передавать разобранный материал в функцию, заданную пользователем. В [SBJson](http://cocoadocs.org/docsets/SBJson/4.0.0/Classes/SBJson4Parser.html) и вообще в Objective-C этого можно добиться с помощью blocks, а в C, наверное, для этого можно использовать void * pointers to function.

> ...идея о том, что вместо самостоятельного создания JSON-объектов
вызывать функции юзера — реально крутая. Потому что юзеру ведь нужны не
нетипизированные JSON-объекты, а свои, так зачем заставлять его снова
парсить наш объект? Обязательно надо реализовать в версии 2.0.

* Да, вам ещё надо позаботиться об ошибках. Исключения — это круто, но
не в C (ручное управление памятью и исключения не очень-то дружат),
значит, вам надо уметь сообщать об ошибках, возвращая дескриптор ошибки.
Кроме сообщения "Ой, у меня тут должна быть запятая, а встретилось фиг
знает что", надо бы указать контекст: в какой позиции? Какой самый
внутренний разбираемый объект, и где его начало? Это не такая простая
задача, поверьте.

* Нужно выбрать стандарт: C99 или ANSI?

## Примеры использования будущих API

* На примере TypicalJSON:

1. Разобрать исходную строку;
2. Получить данные root-Object по ключу "data";
3. Пройти по всем Object, представляющим пользователей.
4. Для каждого такого Object, пройти по всем парам {"имя":"соответствующее ему значение"}.
5. Записать эти значения в БД или соответствующие им по структуре
   объекты Objective-C.

> В реальной практике этот проход нужен для наполнения данными соответствующих записей объектов вроде User с соответствующими полями (id, email, name, ...) и, например, записью этих объектов в БД (В Objective-C эти объекты основаны на структурах struct).

## Примеры JSON для вдохновения

* [Typical JSON](https://github.com/stanislaw/spj/blob/master/TypicalJSON.md)

Типичный JSON для REST API с просторов инета: { "data": [ массив пользователей ] }, при этом массив может быть 1, 10, 100, до, например, 1000 пользователей (можно для пристрастия увеличить гипотетически до 10000). Каждый пользователь в соотв. ему Object может иметь до 20 полей: идентификатор, имя, ник, ... 

Некоторые REST API могут отдавать вместо ключа _"data":_ например _"users":_ или даже вовсе убирать Object и просто присылать массив пользователей, то есть в этих случаях количество уровней вложенности становится ещё меньше.

* `curl http://hashcode.ru/api/json/deleted_questions?startAt=1&amount=1`

## Ссылки

* [Toy parser and lexer на C++](https://github.com/stanislaw/spj/blob/master/ParserAndLexer.md)
 
