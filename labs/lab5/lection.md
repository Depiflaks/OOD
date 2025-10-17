# Pattern "Command"

## Стурктура паттерна
1) Resiever - получатель. Объект, над которым выполняются действия, методы которого используют команды

2) ICommand Command - команда, которая знает о получателе и выполняет над ним операции

3) Invoker - Инициатор. Хранит внутри себя команды и отвечает за то, чтобы их вызывать

4) Client - Клиент. Создаёт команды, дружит их с Получателем и Инициаотором
*При этом Invoker никак не должен быть связан с Resiever*


## Пример
Система управления роботом

1) Робот - Resiever
```mmd
class Robot {
    + TurnOn()
    + TurnOff()
    + Walk(direction)
    + Stop()
}

```

2) Система распознавания речи - Invoker
    - Предоставляет api для связи фраз с произвольной командой

```mmd
class Menu {
    + AddItem(shortcut, command)
    + Run()
    + ShowInstructions() const
    + Exit()
}
```

3) Команды

```
class ICommand {
    +Execute
}
```
Команды могут управлять разными Resiever получателями

4) Клиент



## Функциональная реализация
Вместо ICommand использовать std::function<void()>


## Макро-команды
Паттерн "Компановщик"

## Отмена операций
```
    + Unexecute()

```

- Нужно будет хранить дополнительные данные. Может помочь паттерн Memento (хранитель)
- Отмена макрокоманд: *в обратном порядке*
- Реализовать управление историей изменений

```
class History {
    + CanUndo() bool
    + CanRedo() bool
    + Undo() void
    + Redo() void
    + AddAndExecuteCommand(command)
    - m_commands
    - currentActoinIndex
}
```
Паттерн шаблонный метод - то, что использовали в паттерне декоратор

## Склейка команд
1) Разница времени ввода
2) Однотипные команды, которые идут подряд
    - как узнать, что команды однотипные? 

```
class IUndoableEdit { 
    + CanUndo()
    + CanRedo()
    + Undo()
    + Redo()
    + Destroy() %% отвечает за уничтожение
    + AddEdit(IUndoableEdit) bool %% внутрь команды добавить другую команду (макро команды)
    + ReplaceEdit(IUndoableEdit) bool
    + GetName() string
}

class AbstractUndoableEdit {
}

class ConcreteEdit {
}

class CompoundEdit {
    + AddEdit()
    + End()
    - m_edits
    - m_inProgress: bool
}

```

Как работает: сохраняет внутри себя историю

То есть для каждой новой команды создаём новый экземпляр команды
