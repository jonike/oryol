//------------------------------------------------------------------------------
//  RunLoop.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RunLoop.h"

namespace Oryol {
namespace Core {

OryolClassImpl(RunLoop);

//------------------------------------------------------------------------------
RunLoop::RunLoop() :
curId(InvalidId)
{
    // empty
}

//------------------------------------------------------------------------------
RunLoop::~RunLoop() {
    // empty
}

//------------------------------------------------------------------------------
void
RunLoop::Run() {
    this->remCallbacks();
    this->addCallbacks();
    for (const auto& entry : this->callbacks) {
        if (entry.Value().valid) {
            entry.Value().func();
        }
    }
    this->remCallbacks();
    this->addCallbacks();
}

//------------------------------------------------------------------------------
bool
RunLoop::HasCallback(Id id) const {
    return this->callbacks.Contains(id) || this->toAdd.Contains(id);
}

//------------------------------------------------------------------------------
/**
 NOTE: the callback function will not be added immediately, but at the
 start or end of the Run function.
*/
RunLoop::Id
RunLoop::Add(Func func) {
    Id newId = ++this->curId;
    this->toAdd.Insert(newId, item{func, false});
    return newId;
}

//------------------------------------------------------------------------------
/**
 NOTE: the callback function not be removed immediately, but at the 
 start or end of the Run function.
*/
void
RunLoop::Remove(Id id) {
    o_assert_dbg(!this->toRemove.Contains(id));
    o_assert_dbg(this->callbacks.Contains(id));
    this->toRemove.Insert(id);
}

//------------------------------------------------------------------------------
void
RunLoop::addCallbacks() {
    for (auto& entry : this->toAdd) {
        item& item = entry.Value();
        item.valid = true;
        this->callbacks.Insert(entry.Key(), item);
    }
    this->toAdd.Clear();
}

//------------------------------------------------------------------------------
void
RunLoop::remCallbacks() {
    for (Id id : this->toRemove) {
        this->callbacks.Erase(id);
    }
    this->toRemove.Clear();
}

} // namespace Core
} // namespace Oryol