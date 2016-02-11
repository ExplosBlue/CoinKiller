#ifndef IS
#define IS

template <class Derived, class Base> bool is(Base base)
{
   return dynamic_cast<Derived>(base) != nullptr;
}

#endif // IS

