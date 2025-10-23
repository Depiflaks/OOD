#ifndef HISTORY_H
#define HISTORY_H

class IHistoryManager
{
public:
	virtual ~IHistoryManager() = default;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
	virtual bool CanUndo() const = 0;
	virtual bool CanRedo() const = 0;
};

#endif /* HISTORY_H */
