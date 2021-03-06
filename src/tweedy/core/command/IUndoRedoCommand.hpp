#ifndef IUNDOREDOCOMMAND_HPP
#define	IUNDOREDOCOMMAND_HPP

#include <boost/noncopyable.hpp>

#include <string>

class IUndoRedoCommand : private boost::noncopyable
{
protected:
    IUndoRedoCommand(){}
    IUndoRedoCommand( const IUndoRedoCommand& ){}
    
public:
    virtual ~IUndoRedoCommand()=0;
    
    virtual IUndoRedoCommand* clone() const = 0;
    
    virtual void undo()= 0;
    virtual void runDo()=0;
    virtual void redo()= 0;
    virtual const std::string& getText() const =0;
};

inline IUndoRedoCommand::~IUndoRedoCommand(){}

inline IUndoRedoCommand* new_clone(const IUndoRedoCommand& i){
    return i.clone();
}

#endif	/* IUNDOREDOCOMMAND_HPP */

