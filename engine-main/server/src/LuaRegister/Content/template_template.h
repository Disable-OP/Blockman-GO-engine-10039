// Register Class function_traits_impl
/*
_BEGIN_REGISTER_CLASS(function_traits_impl)
_END_REGISTER_CLASS()


// Register Class BitSet
_BEGIN_REGISTER_CLASS(BitSet)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(assign, BitSet::assign)
_CLASSREGISTER_AddMember(clear, BitSet::clear)
_CLASSREGISTER_AddMember(dirty, BitSet::dirty)
_CLASSREGISTER_AddMember(flags, BitSet::flags)
_CLASSREGISTER_AddMember(flip, BitSet::flip)
_CLASSREGISTER_AddMember(lags, BitSet::lags)
_CLASSREGISTER_AddMember(map, BitSet::map)
_CLASSREGISTER_AddMember_Override(reset, BitSet::reset,void ,int )
_CLASSREGISTER_AddMember_Override(reset1, BitSet::reset,void )
_CLASSREGISTER_AddMember_Override(set, BitSet::set,void ,int )
_CLASSREGISTER_AddMember_Override(set1, BitSet::set,void )
_END_REGISTER_CLASS()


// Register Class Future
_BEGIN_REGISTER_CLASS(Future)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(cancel, Future::cancel)
_CLASSREGISTER_AddMember(get, Future::get)
_CLASSREGISTER_AddMember(getStatus, Future::getStatus)
_CLASSREGISTER_AddMember(isCanceled, Future::isCanceled)
_CLASSREGISTER_AddMember(isFinished, Future::isFinished)
_END_REGISTER_CLASS()


// Register Class Listenable
_BEGIN_REGISTER_CLASS(Listenable)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(const VariableType & )
_CLASSREGISTER_AddCtor(const Listenable & )
_CLASSREGISTER_AddMember(subscribe, Listenable::subscribe)
_END_REGISTER_CLASS()


// Register Class Subscription
_BEGIN_REGISTER_CLASS(Subscription)
_CLASSREGISTER_AddCtor(const Iterator & , <SubscriberList> )
_CLASSREGISTER_AddMember(unsubscribe, Subscription::unsubscribe)
_END_REGISTER_CLASS()


// Register Class SubscriptionData
_BEGIN_REGISTER_CLASS(SubscriptionData)
_CLASSREGISTER_AddCtor(const Iterator & , <SubscriberList> )
_END_REGISTER_CLASS()


// Register Class RingQueue
_BEGIN_REGISTER_CLASS(RingQueue)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(capacity, RingQueue::capacity)
_CLASSREGISTER_AddMember(drop, RingQueue::drop)
_CLASSREGISTER_AddMember(free, RingQueue::free)
_CLASSREGISTER_AddMember(peek_front, RingQueue::peek_front)
_CLASSREGISTER_AddMember(pop_front, RingQueue::pop_front)
_CLASSREGISTER_AddMember(push_back, RingQueue::push_back)
_CLASSREGISTER_AddMember(push_back_begin, RingQueue::push_back_begin)
_CLASSREGISTER_AddMember(push_back_end, RingQueue::push_back_end)
_CLASSREGISTER_AddMember(push_back_fast, RingQueue::push_back_fast)
_CLASSREGISTER_AddMember(used, RingQueue::used)
_END_REGISTER_CLASS()


// Register Class Creater
_BEGIN_REGISTER_CLASS(Creater)
_CLASSREGISTER_AddStaticMember(create, Creater::create)
_END_REGISTER_CLASS()


// Register Class UFactory
_BEGIN_REGISTER_CLASS(UFactory)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(UFactory & )
_CLASSREGISTER_AddMember(create, UFactory::create)
_CLASSREGISTER_AddMember(registerCreator, UFactory::registerCreator)
_CLASSREGISTER_AddMember(unregisterAllCreators, UFactory::unregisterAllCreators)
_END_REGISTER_CLASS()


// Register Class ThreadSafeQueue
_BEGIN_REGISTER_CLASS(ThreadSafeQueue)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(empty, ThreadSafeQueue::empty)
_CLASSREGISTER_AddMember(push, ThreadSafeQueue::push)
_CLASSREGISTER_AddMember(size, ThreadSafeQueue::size)
_CLASSREGISTER_AddMember(try_pop, ThreadSafeQueue::try_pop)
_CLASSREGISTER_AddMember(wait_and_pop, ThreadSafeQueue::wait_and_pop)
_END_REGISTER_CLASS()
*/

