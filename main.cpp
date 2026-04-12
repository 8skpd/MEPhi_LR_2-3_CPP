#include <iostream>
#include <string>
#include <limits>
#include "ArraySequence.h"
#include "ListSequence.h"
#include "Deque.h"
#include "Converters.h" 

// Простые функции для тестирования
int testsPassed = 0;
int testsFailed = 0;

void test(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
        testsPassed++;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
        testsFailed++;
    }
}

void printSection(const std::string& name) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       " << name << std::endl;
    std::cout << "========================================" << std::endl;
}


void testDynamicArray() {
    printSection("LR2: DynamicArray Tests");
    
    int arr[] = {1, 2, 3};
    DynamicArray<int>* da = new DynamicArray<int>(arr, 3);
    
    test(da->GetSize() == 3, "Size after creation");
    test(da->Get(0) == 1, "Get first element");
    test(da->Get(2) == 3, "Get last element");
    
    da->Resize(5);
    test(da->GetSize() == 5, "Resize up");
    test(da->Get(3) == 0, "New elements initialized to 0");
    
    da->Set(4, 99);
    test(da->Get(4) == 99, "Set element");
    
    delete da;
}

void testLinkedList() {
    printSection("LR2: LinkedList Tests");
    
    LinkedList<int>* list = new LinkedList<int>();
    list->Append(10);
    list->Append(20);
    list->Prepend(5);
    
    test(list->GetLength() == 3, "List length");
    test(list->GetFirst() == 5, "First element");
    test(list->GetLast() == 20, "Last element");
    test(list->Get(1) == 10, "Middle element");
    
    delete list;
}

void testSequenceBasics() {
    printSection("LR2: Sequence Basics (Array & List)");
    
    int arr[] = {1, 2, 3, 4, 5};
    Sequence<int>* seqArr = new MutableArraySequence<int>(arr, 5);
    Sequence<int>* seqList = new MutableListSequence<int>(arr, 5);
    
    // Тесты ArraySequence
    test(seqArr->GetLength() == 5, "ArraySeq Length");
    test(seqArr->GetFirst() == 1, "ArraySeq First");
    test(seqArr->GetLast() == 5, "ArraySeq Last");
    test(seqArr->Get(2) == 3, "ArraySeq Get index");
    
    // Тесты ListSequence
    test(seqList->GetLength() == 5, "ListSeq Length");
    test(seqList->Get(2) == 3, "ListSeq Get index");
    test(seqList->GetFirst() == 1, "ListSeq First");
    test(seqList->GetLast() == 5, "ListSeq Last");
    
    
    // 1. Проверка выхода за границы (должно вернуть None)
    Option<int> optInvalid = seqArr->TryGet(10);
    test(optInvalid.IsNone(), "TryGet invalid index returns None");
    test(!optInvalid, "TryGet invalid index operator bool is false");

    // 2. Проверка валидного индекса (должно вернуть Some)
    Option<int> optValid = seqArr->TryGet(0);
    test(optValid.IsSome(), "TryGet valid index returns Some");
    test(optValid.GetValue() == 1, "TryGet valid index value is correct");

    // 3. Проверка TryFirst / TryLast на непустой последовательности
    test(seqArr->TryFirst().GetValue() == 1, "TryFirst returns correct value");
    test(seqArr->TryLast().GetValue() == 5, "TryLast returns correct value");

    // 4. Проверка TryFirst / TryLast на пустой последовательности
    Sequence<int>* emptySeq = new MutableArraySequence<int>();
    
    Option<int> optEmptyFirst = emptySeq->TryFirst();
    test(optEmptyFirst.IsNone(), "TryFirst on empty returns None");

    Option<int> optEmptyLast = emptySeq->TryLast();
    test(optEmptyLast.IsNone(), "TryLast on empty returns None");
    
    Option<int> optEmptyGet = emptySeq->TryGet(0);
    test(optEmptyGet.IsNone(), "TryGet on empty returns None");

    delete emptySeq;

    delete seqArr;
    delete seqList;
}

void testMapReduceWhere() {
    printSection("LR2: Map, Reduce, Where");
    int arr[] = {1, 2, 3, 4, 5};
    // Создаем как конкретный тип
    MutableArraySequence<int>* seq = new MutableArraySequence<int>(arr, 5);
    
    // Возвращаем как базовый тип (или auto)
    Sequence<int>* mapped = seq->Map([](int x) { return x * 2; });

    if (mapped == nullptr) {
        std::cout << "[FAIL] Map returned nullptr!" << std::endl;
    } else {
        test(mapped->GetLength() == 5, "Map length");
        test(mapped->Get(0) == 2, "Map first element");
    }
    
    delete seq;
    delete mapped;
}

void testZipAndIterators() {
    printSection("LR2: Zip and Iterators");
    
    int arr1[] = {1, 2, 3};
    char arr2[] = {'a', 'b', 'c'};
    
    Sequence<int>* seqInt = new MutableArraySequence<int>(arr1, 3);
    Sequence<char>* seqChar = new MutableArraySequence<char>(arr2, 3);

    BaseArraySequence<int>* arrInt = dynamic_cast<BaseArraySequence<int>*>(seqInt);
    BaseArraySequence<char>* arrChar = dynamic_cast<BaseArraySequence<char>*>(seqChar);

    Sequence<Pair<int, char>>* zipped = nullptr;

    if (arrInt && arrChar) {
        zipped = arrInt->Zip(arrChar); // Присваиваем внутри
        
        test(zipped->GetLength() == 3, "Zip length");
        test(zipped->Get(0).first == 1 && zipped->Get(0).second == 'a', "Zip first pair");
        test(zipped->Get(2).first == 3 && zipped->Get(2).second == 'c', "Zip last pair");
        
        delete zipped;
    } else {
        test(false, "Dynamic cast failed in Zip test");
    }
    // Iterator
    auto enumerator = seqInt->GetEnumerator();
    int count = 0;
    int sum = 0;
    while (enumerator->MoveNext()) {
        sum += enumerator->GetCurrent();
        count++;
    }
    test(count == 3, "Iterator count");
    test(sum == 6, "Iterator sum");
    delete enumerator;

    delete seqInt;
    delete seqChar;
}

void testIterators() {
    printSection("LR2: Iterators");

    // Итератор для массива
    {
        int arr[] = {10, 20, 30};
        Sequence<int>* seq = new ImmutableArraySequence<int>(arr, 3);
        auto enumerator = seq->GetEnumerator();
        
        int count = 0;
        int sum = 0;
        
        // Проход по всем элементам
        while (enumerator->MoveNext()) {
            sum += enumerator->GetCurrent();
            count++;
        }
        
        test(count == 3, "Array Iterator: count elements");
        test(sum == 60, "Array Iterator: sum elements");
        
        // Проверка поведения после завершения цикла
        test(!enumerator->MoveNext(), "Array Iterator: MoveNext returns false at end");
        
        delete seq;
        delete enumerator;
    }

    // Итератор для списка
    {
        int arr[] = {5, 15, 25, 35};
        Sequence<int>* seq = new MutableListSequence<int>(arr, 4);
        auto enumerator = seq->GetEnumerator();
        
        int count = 0;
        int product = 1;
        
        while (enumerator->MoveNext()) {
            product *= enumerator->GetCurrent();
            count++;
        }
        
        test(count == 4, "List Iterator: count elements");
        test(product == 5 * 15 * 25 * 35, "List Iterator: product elements");
        
        delete seq;
        delete enumerator;
    }

    // Итератор для пустой последовательности
    {
        Sequence<int>* seq = new MutableArraySequence<int>(); // Пустой дек
        auto enumerator = seq->GetEnumerator();
        
        // Первый вызов MoveNext должен вернуть false
        bool hasElements = enumerator->MoveNext();
        test(!hasElements, "Empty Iterator: MoveNext returns false immediately");
        
        delete seq;
        delete enumerator;
    }
    
    // Перезапуск итератора
    {
        int arr[] = {1, 2};
        Sequence<int>* seq = new ImmutableArraySequence<int>(arr, 2);
        auto enumerator = seq->GetEnumerator();
        
        // Пробегаем один раз
        enumerator->MoveNext(); 
        int first = enumerator->GetCurrent();
        enumerator->MoveNext();
        
        // Сбрасываем
        enumerator->Reset();
        
        // Должны снова получить первый элемент
        test(enumerator->MoveNext(), "Reset: MoveNext works after reset");
        test(enumerator->GetCurrent() == first, "Reset: GetCurrent returns first element");
        
        delete seq;
        delete enumerator;
    }
}

void testConverters() {
    printSection("Testing Converters (Array <-> List)");

    int arr[] = {10, 20, 30, 40, 50};
    const int size = 5;

    {        
        // Создаем исходный массив
        Sequence<int>* arraySeq = new MutableArraySequence<int>(arr, size);
        
        // Конвертируем в список
        Sequence<int>* listSeq = ConvertToListSequence(arraySeq);
        
        // Проверки
        test(listSeq->GetLength() == size, "ConvertToArray->List: Length matches");
        
        bool valuesMatch = true;
        for (int i = 0; i < size; ++i) {
            if (listSeq->Get(i) != arr[i]) {
                valuesMatch = false;
                break;
            }
        }
        test(valuesMatch, "ConvertToArray->List: Values match");
        
        // Проверка типа через dynamic_cast
        test(dynamic_cast<BaseListSequence<int>*>(listSeq) != nullptr, 
             "ConvertToArray->List: Result is actually a ListSequence");
        test(dynamic_cast<BaseArraySequence<int>*>(listSeq) == nullptr, 
             "ConvertToArray->List: Result is NOT an ArraySequence");

        delete arraySeq;
        delete listSeq;
    }

    {
        
        // Создаем исходный список
        Sequence<int>* listSeq = new MutableListSequence<int>(arr, size);
        
        // Конвертируем в массив
        Sequence<int>* arraySeq = ConvertToArraySequence(listSeq);
        
        // Проверки
        test(arraySeq->GetLength() == size, "ConvertList->Array: Length matches");
        
        bool valuesMatch = true;
        for (int i = 0; i < size; ++i) {
            if (arraySeq->Get(i) != arr[i]) {
                valuesMatch = false;
                break;
            }
        }
        test(valuesMatch, "ConvertList->Array: Values match");
        
        // Проверка типа
        test(dynamic_cast<BaseArraySequence<int>*>(arraySeq) != nullptr, 
             "ConvertList->Array: Result is actually an ArraySequence");
        test(dynamic_cast<BaseListSequence<int>*>(arraySeq) == nullptr, 
             "ConvertList->Array: Result is NOT a ListSequence");

        delete listSeq;
        delete arraySeq;
    }

    {
        
        // Массив -> Список
        Sequence<int>* sourceArray = new MutableArraySequence<int>(arr, size);
        Sequence<int>* oppositeFromArr = ConvertToOpposite(sourceArray);
        
        test(dynamic_cast<BaseListSequence<int>*>(oppositeFromArr) != nullptr, 
             "Opposite: Array became List");
        test(oppositeFromArr->Get(0) == 10 && oppositeFromArr->Get(4) == 50, 
             "Opposite: Data preserved (Array->List)");
        
        delete sourceArray;
        delete oppositeFromArr;

        // Список -> Массив
        Sequence<int>* sourceList = new MutableListSequence<int>(arr, size);
        Sequence<int>* oppositeFromList = ConvertToOpposite(sourceList);
        
        test(dynamic_cast<BaseArraySequence<int>*>(oppositeFromList) != nullptr, 
             "Opposite: List became Array");
        test(oppositeFromList->Get(0) == 10 && oppositeFromList->Get(4) == 50, 
             "Opposite: Data preserved (List->Array)");
        
        delete sourceList;
        delete oppositeFromList;
    }

    {
        
        Sequence<int>* emptyArray = new MutableArraySequence<int>();
        Sequence<int>* convertedEmpty = ConvertToListSequence(emptyArray);
        
        test(convertedEmpty->GetLength() == 0, "Converter: Handles empty sequence");
        test(dynamic_cast<BaseListSequence<int>*>(convertedEmpty) != nullptr, 
             "Converter: Empty Array -> Empty List type correct");
        
        delete emptyArray;
        delete convertedEmpty;
    }
}

void testCustomTuple() {
    printSection("Testing Custom Tuple & MakeTuple");

    {
        
        // Создаем кортеж из 3 разных типов
        auto t = MakeTuple(10, 3.14, std::string("Hello"));
        
        // Проверка типов и значений через get<N>
        test(get<0>(t) == 10, "Tuple: Get int (index 0)");
        test(get<1>(t) == 3.14, "Tuple: Get double (index 1)");
        test(get<2>(t) == "Hello", "Tuple: Get string (index 2)");
    }

    {
        auto t = MakeTuple(1, 2, 3);
        
        // Изменяем значения через ссылку, возвращаемую get<>
        get<0>(t) = 100;
        get<1>(t) = 200;
        get<2>(t) = 300;
        
        test(get<0>(t) == 100, "Tuple: Modify first element");
        test(get<1>(t) == 200, "Tuple: Modify second element");
        test(get<2>(t) == 300, "Tuple: Modify third element");
    }

    {
        
        auto t = MakeTuple(10, 20, 30, 40, 50);
        
        test(get<0>(t) == 10, "Tuple 5: Element 0");
        test(get<2>(t) == 30, "Tuple 5: Element 2 (middle)");
        test(get<4>(t) == 50, "Tuple 5: Element 4 (last)");
            }


    {        
        int val = 42;
        auto t = MakeTuple('A', true, &val);
        
        test(get<0>(t) == 'A', "Tuple: Char element");
        test(get<1>(t) == true, "Tuple: Bool element");
        test(*get<2>(t) == 42, "Tuple: Pointer dereference");
        
        // Меняем значение через указатель внутри кортежа
        *get<2>(t) = 99;
        test(val == 99, "Tuple: Pointer modification affects original");
    }

    {        
        auto t = MakeTuple();
        // Компиляция этого кода уже является тестом того, что пустой шаблон работает
        test(true, "Tuple: Empty tuple compiles and constructs");
    }

    {
        std::string s1 = "Original";
        std::string s2 = "Temporary";
        
        // Передаем lvalue (должно скопироваться)
        auto t1 = MakeTuple(s1);
        s1 = "Modified"; // Меняем оригинал
        test(get<0>(t1) == "Original", "Tuple: Lvalue copied correctly");
        
        // Передаем rvalue (должно переместиться)
        auto t2 = MakeTuple(std::move(s2));
        // После move s2 должен быть в неопределенном состоянии (обычно пуст), но данные в кортеже
        test(get<0>(t2) == "Temporary", "Tuple: Rvalue moved correctly");
        test(s2.empty(), "Tuple: Original string emptied after move");
    }

    {        
        const auto t = MakeTuple(100, 200.5);
        
        test(get<0>(t) == 100, "Const Tuple: Read int");
        test(get<1>(t) == 200.5, "Const Tuple: Read double");
        
        // Следующая строка должна вызвать ошибку компиляции, если раскомментировать:
        // get<0>(t) = 500; 
        test(true, "Const Tuple: Compilation check");
    }

}

void testEnsureMethods() {
    printSection("LR2: EnsureArray & EnsureList Methods");

    int arr[] = {10, 20, 30, 40, 50};
    const int size = 5;

    // Тест 1: List -> EnsureArray (Конвертация)
    {
        Sequence<int>* listSeq = new MutableListSequence<int>(arr, size);
        Sequence<int>* result = listSeq->EnsureArray();
        
        test(dynamic_cast<BaseArraySequence<int>*>(result) != nullptr, "EnsureArray: Result is ArraySequence");
        test(result->GetLength() == size, "EnsureArray: Length preserved");
        test(result->Get(0) == 10 && result->Get(4) == 50, "EnsureArray: Data preserved");
        
        // Так как тип изменился, указатели разные -> удаляем оба
        delete listSeq;
        delete result;
    }

    // Тест 2: Array -> EnsureArray (Оптимизация / Same Pointer)
    {
        Sequence<int>* arraySeq = new MutableArraySequence<int>(arr, size);
        Sequence<int>* result = arraySeq->EnsureArray();
        
        // Проверяем, что указатели совпадают (копия не создавалась)
        test(result == arraySeq, "EnsureArray on Array: Returns same pointer");
        test(dynamic_cast<BaseArraySequence<int>*>(result) != nullptr, "EnsureArray on Array: Type correct");
        
        // Удаляем только один раз
        delete arraySeq; 
    }

    // Тест 3: Array -> EnsureList (Конвертация)
    {
        Sequence<int>* arraySeq = new MutableArraySequence<int>(arr, size);
        Sequence<int>* result = arraySeq->EnsureList();
        
        test(dynamic_cast<BaseListSequence<int>*>(result) != nullptr, "EnsureList: Result is ListSequence");
        test(result->GetLength() == size, "EnsureList: Length preserved");
        test(result->Get(2) == 30, "EnsureList: Middle element correct");
        
        delete arraySeq;
        delete result;
    }

    // Тест 4: List -> EnsureList (Оптимизация)
    {
        Sequence<int>* listSeq = new MutableListSequence<int>(arr, size);
        Sequence<int>* result = listSeq->EnsureList();
        
        test(result == listSeq, "EnsureList on List: Returns same pointer");
        test(dynamic_cast<BaseListSequence<int>*>(result) != nullptr, "EnsureList on List: Type correct");
        
        delete listSeq;
    }

    // Тест 5: Методы-синонимы ToArray и ToList
    {
        Sequence<int>* listForArray = new MutableListSequence<int>(arr, 3);
        Sequence<int>* arrRes = listForArray->ToArray();
        test(dynamic_cast<BaseArraySequence<int>*>(arrRes) != nullptr, "ToArray: Converts to Array");
        delete listForArray;
        delete arrRes;

        Sequence<int>* arrayForList = new MutableArraySequence<int>(arr, 3);
        Sequence<int>* listRes = arrayForList->ToList();
        test(dynamic_cast<BaseListSequence<int>*>(listRes) != nullptr, "ToList: Converts to List");
        delete arrayForList;
        delete listRes;
    }

    // Тест 6: Пустая последовательность
    {
        Sequence<int>* emptyList = new MutableListSequence<int>();
        Sequence<int>* emptyArr = emptyList->EnsureArray();
        
        test(emptyArr->GetLength() == 0, "EnsureArray on Empty: Length is 0");
        test(dynamic_cast<BaseArraySequence<int>*>(emptyArr) != nullptr, "EnsureArray on Empty: Type is Array");
        
        delete emptyList;
        delete emptyArr;
    }
}

void runAllLR2Tests() {
    printSection("STARTING LR2 TESTS");
    testDynamicArray();
    testLinkedList();
    testSequenceBasics();
    testMapReduceWhere();
    testZipAndIterators();
    testIterators();
    testConverters();
    testCustomTuple();
    testEnsureMethods();
    
    std::cout << "\n>>> LR2 Summary: " << testsPassed << " passed, " << testsFailed << " failed." << std::endl;
}


void testDequeBasics() {
    printSection("LR3: Deque Basic Operations");
    
    Deque<int> dq;
    
    // Push Back
    dq.PushBack(1);
    dq.PushBack(2);
    dq.PushBack(3);
    test(dq.GetLength() == 3, "Deque size after PushBack");
    test(dq.GetFront() == 1, "Front element");
    test(dq.GetBack() == 3, "Back element");
    
    // Push Front
    dq.PushFront(0);
    test(dq.GetLength() == 4, "Size after PushFront");
    test(dq.GetFront() == 0, "New Front");
    
    // Pop Front
    int val = dq.PopFront();
    test(val == 0, "PopFront value");
    test(dq.GetLength() == 3, "Size after PopFront");
    test(dq.GetFront() == 1, "New Front after pop");
    
    // Pop Back
    val = dq.PopBack();
    test(val == 3, "PopBack value");
    test(dq.GetLength() == 2, "Size after PopBack");
    test(dq.GetBack() == 2, "New Back after pop");
}

void testDequeExceptions() {
    printSection("LR3: Deque Exceptions");
    
    Deque<int> dq;
    bool thrown = false;
    
    try { dq.PopFront(); } catch (...) { thrown = true; }
    test(thrown, "PopFront on empty throws");
    
    thrown = false;
    try { dq.PopBack(); } catch (...) { thrown = true; }
    test(thrown, "PopBack on empty throws");
    
    thrown = false;
    try { dq.GetFront(); } catch (...) { thrown = true; }
    test(thrown, "GetFront on empty throws");

    // Single element test
    dq.PushBack(42);
    try {
        int val = dq.PopFront();
        test(val == 42, "Pop single element");
        test(dq.GetLength() == 0, "Empty after popping single");
    } catch (...) {
        test(false, "Pop single element threw unexpectedly");
    }
}

void testDequeSortAndMerge() {
    printSection("LR3: Deque Sort and Merge");
    
    // Sort
    int arr[] = {5, 1, 9, 2, 8};
    Deque<int> dq(arr, 5);
    dq.Sort();
    
    test(dq[0] == 1, "Sorted first");
    test(dq[4] == 9, "Sorted last");
    test(dq[2] == 5, "Sorted middle");
    
    // Merge
    int arr1[] = {1, 3, 5};
    int arr2[] = {2, 4, 6, 8};
    Deque<int> dq1(arr1, 3);
    Deque<int> dq2(arr2, 4);
    
    dq1.Sort(); // На всякий случай
    dq2.Sort();
    
    dq1.Merge(dq2);
    test(dq1.GetLength() == 7, "Merged size");
    test(dq1[3] == 4, "Merged middle element");
    test(dq1[6] == 8, "Merged last element");
}

void testDequeOperators() {
    printSection("LR3: Deque Operators");
    
    int arr1[] = {1, 2};
    int arr2[] = {3, 4};
    Deque<int> dq1(arr1, 2);
    Deque<int> dq2(arr2, 2);
    
    // Operator +
    Deque<int> dq3 = dq1 + dq2;
    test(dq3.GetLength() == 4, "Concat size");
    test(dq3[2] == 3, "Concat element");
    
    // Operator ==
    Deque<int> dq1Copy = dq1;
    test(dq1 == dq1Copy, "Equality operator");
    test(!(dq1 == dq2), "Inequality operator");
    
    // Operator [] write
    dq1[1] = 99;
    test(dq1[1] == 99, "Write via []");
}

void testDequeAlgorithms() {
    printSection("LR3: Deque Algorithms");
    
    // Count Inversions
    int arr[] = {3, 1, 4, 1, 5};
    Deque<int> dq(arr, 5);
    // Инверсии: (3,1), (3,1), (4,1) -> 3
    test(dq.CountInversions() == 3, "Count Inversions");
    
    // Range
    auto rangeDq = Deque<int>::Range(5, 9);
    test(rangeDq.GetLength() == 5, "Range size");
    test(rangeDq.GetBack() == 9, "Range last");
    
    // Contains Subsequence
    int subArr[] = {1, 4, 1};
    Deque<int> sub(subArr, 3);
    test(dq.ContainsSubsequence(sub), "Contains subsequence");
    
    int badSubArr[] = {1, 5, 3};
    Deque<int> badSub(badSubArr, 3);
    test(!dq.ContainsSubsequence(badSub), "Not contains subsequence");
}

void testDequeMapReduce() {
    printSection("LR3: Deque Map-Reduce");
    
    int arr[] = {1, 2, 3, 4, 5};
    Deque<int> dq(arr, 5);
    
    // Map
    auto mapped = dq.Map([](int x) { return x * 10; });
    test(mapped.GetLength() == 5, "Deque Map length");
    test(mapped[1] == 20, "Deque Map value");
    
    // Where
    auto filtered = dq.Where([](int x) { return x % 2 == 0; });
    test(filtered.GetLength() == 2, "Deque Where count");
    test(filtered[0] == 2, "Deque Where first");
    
    // Reduce
    int sum = dq.Reduce([](int acc, int x) { return acc + x; }, 0);
    test(sum == 15, "Deque Reduce sum");
}

void runAllLR3Tests() {
    printSection("STARTING LR3 TESTS");
    testDequeBasics();
    testDequeExceptions();
    testDequeSortAndMerge();
    testDequeOperators();
    testDequeAlgorithms();
    testDequeMapReduce();
    
    std::cout << "\n>>> LR3 Summary: " << testsPassed << " passed, " << testsFailed << " failed." << std::endl;
}

// Простой консольный интерфейс
void runUI() {
    printSection("DEQUE USER INTERFACE");
    
    Deque<int> dq;
    int choice, value;
    
    while (true) {
        std::cout << "\n--- Menu" << std::endl;
        std::cout << "1. Push Front" << std::endl;
        std::cout << "2. Push Back" << std::endl;
        std::cout << "3. Pop Front" << std::endl;
        std::cout << "4. Pop Back" << std::endl;
        std::cout << "5. Show Front/Back" << std::endl;
        std::cout << "6. Sort" << std::endl;
        std::cout << "7. Count Inversions" << std::endl;
        std::cout << "8. Show All" << std::endl;
        std::cout << "9. Exit" << std::endl;
        std::cout << "Choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        
        try {
            switch (choice) {
                case 1:
                    std::cout << "Value: "; 
                    if (std::cin >> value) dq.PushFront(value);
                    else { std::cin.clear(); std::cin.ignore(10000, '\n'); std::cout << "Invalid number." << std::endl; }
                    break;
                case 2:
                    std::cout << "Value: "; 
                    if (std::cin >> value) dq.PushBack(value);
                    else { std::cin.clear(); std::cin.ignore(10000, '\n'); std::cout << "Invalid number." << std::endl; }
                    break;
                case 3:
                    std::cout << "Popped: " << dq.PopFront() << std::endl;
                    break;
                case 4:
                    std::cout << "Popped: " << dq.PopBack() << std::endl;
                    break;
                case 5:
                    std::cout << "Front: " << dq.GetFront() << ", Back: " << dq.GetBack() << std::endl;
                    break;
                case 6:
                    dq.Sort();
                    std::cout << "Sorted successfully!" << std::endl;
                    break;
                case 7:
                    std::cout << "Inversions count: " << dq.CountInversions() << std::endl;
                    break;
                case 8:
                    std::cout << "Deque: " << dq << std::endl;
                    break;
                case 9:
                    std::cout << "Exiting..." << std::endl;
                    return;
                default:
                    std::cout << "Invalid choice. Please select 1-9." << std::endl;
            }
        } 
        // Перехват исключений
        catch (const EmptyStructureException& e) {
            std::cout << "Error: Cannot perform operation on empty deque!" << std::endl;
            // std::cout << "Details: " << e.what() << std::endl; 
        }
        // Перехват других сообщений на всякий случай
        catch (const IndexOutOfRange& e) {
            std::cout << "Error: Index out of range!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Unexpected error: " << e.what() << std::endl;
        }
    }
}


int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   LAB 2 & 3: COMPREHENSIVE TEST SUITE  " << std::endl;
    std::cout << "========================================" << std::endl;

    // Запуск всех тестов
    runAllLR2Tests();
    runAllLR3Tests();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "   TOTAL: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << std::endl;
    std::cout << "========================================" << std::endl;

    if (testsFailed == 0) {
        std::cout << "\n✓ ALL TESTS PASSED! Ready for defense." << std::endl;
    } else {
        std::cout << "\n✗ SOME TESTS FAILED! Check implementation." << std::endl;
    }

    char runUi;
    std::cout << "\nRun User Interface? (y/n): ";
    std::cin >> runUi;
    if (runUi == 'y' || runUi == 'Y') {
        runUI();
    }

    return 0;
}