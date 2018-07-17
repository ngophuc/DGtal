/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#pragma once

/**
 * @file
 * @author Roland Denis (\c denis@math.univ-lyon1.fr )
 * CNRS, Institut Camille Jordan (UMR 5208), Université Lyon 1, France
 *
 * @date 2018/06/22
 *
 * This file is part of the DGtal library.
 */

#if defined(FunctorHolder_RECURSES)
#error Recursive header files inclusion detected in FunctorHolder.h
#else // defined(FunctorHolder_RECURSES)
/** Prevents recursive inclusion of headers. */
#define FunctorHolder_RECURSES

#if !defined FunctorHolder_h
/** Prevents repeated inclusion of headers. */
#define FunctorHolder_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include <type_traits>
#include <functional>
#include <memory>
//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

//////////////////////////////////////////////////////////////////////////////
// Implementation details
namespace
{

  /// Invoke a callable object without dereferencing it before.
  template <bool>
  struct Invoker
  {
    template <typename F, typename... T>
    static inline
    auto apply(F && fn, T &&... args)
        -> decltype((std::forward<F>(fn))(std::forward<T>(args)...))
      {
        return (std::forward<F>(fn))(std::forward<T>(args)...);
      }
  };

  /// Invoke a callable object by dereferencing it before.
  template <>
  struct Invoker<true>
  {
    template <typename F, typename... T>
    static inline
    auto apply(F && fn, T &&... args)
        -> decltype((*std::forward<F>(fn))(std::forward<T>(args)...))
      {
        return (*std::forward<F>(fn))(std::forward<T>(args)...);
      }
  };

  /// Type traits to detect storage based on std::shared_ptr (for debug purpose)
  struct SharedPtrTrait
    {
      template <typename T>
      static constexpr std::true_type   apply(std::shared_ptr<T> const&) { return {}; }

      template <typename T>
      static constexpr std::false_type  apply(T const&) { return {}; }
    };

  /// Type traits to detect storage based on std::reference_wrapper (for debug purpose)
  struct ReferenceWrapperTrait
    {
      template <typename T>
      static constexpr std::true_type   apply(std::reference_wrapper<T> const&) { return {}; }

      template <typename T>
      static constexpr std::false_type  apply(T const&) { return {}; }
    };

} // anonymous namespace


/////////////////////////////////////////////////////////////////////////////
// class FunctorHolder
/**
 * @brief Aim: hold any callable object (function, functor, lambda, ...) as a C(Unary)Functor model.
 *
 * @tparam FunctorStorage  Type used to store the given callable object.
 * @tparam NeedDereference  true if the object must be dereferenced before calls.
 *
 * @warning This class is not meant to be directly constructed by the user.
 * As explained below, you should use instead the @ref holdFunctor helper that
 * will choose the more appropriate storage type depending on the given
 * callable object.
 *
 * @section aim Aim
 *
 * Given any type of callable object (function, functor and lambda) passed
 * by value, reference or pointer, stores it in the most appropriate way so
 * that the returned object is compatible with DGtal functor concept
 * (@ref concepts::CUnaryFunctor so far), and especially the @ref boost::Assignable concept.
 *
 * More precisely, the storage mode depends on the lifetime of the given
 * object, deduced from its passing mode :
 * - if the given object is passed by left-value reference (long lifetime),
 *   it will be stored as a reference (using `std::reference_wrapper`).
 * - otherwise (short lifetime, like right-value references),
 *   it will be moved to a dynamically allocated memory place (`std::shared_ptr`)
 *   so that the object lifetime matches the FunctorHolder lifetime.
 *
 * In both case, the returned object is copy/move constructible and
 * copy/move assignable so that it can be used as a functor in all DGtal
 * library.
 *
 * @remark It is important to not explicitly specify the given object type
 * so that it's real lifetime can be deduced using universal reference.
 * However, you can transfer ownership of the object to FunctorHolder by moving
 * it using `std::move`.
 * As a consequence, the type of the returned FunctorHolder cannot be guessed
 * easily and the use of the `auto` keyword is thus mandatory.
 *
 * @note
 * Almost all the snippets used in the following explanations come from
 * the example file @ref exampleFunctorHolder.cpp
 *
 * @section holding_an_object Holding a callable object
 *
 * As warned before, @ref FunctorHolder is not meant to be directly constructed
 * but instead through the helper (factory) @ref holdFunctor.
 * You can hold any type of callable object :
 *
 * @subsection holding_a_function Holding a function
 *
 * If you want to refer to an existing function, you can directly pass its name
 * to @ref holdFunctor :
 * @snippet exampleFunctorHolder.cpp signed_dist_to_unit_circle
 * @snippet exampleFunctorHolder.cpp Holding a function
 *
 * If the function is templated, you must specify the needed templates:
 * @snippet exampleFunctorHolder.cpp Templated signed_dist_to_unit_circle
 * @snippet exampleFunctorHolder.cpp Holding a templated function
 *
 * In both cases, the function will be passed by reference.
 * You can also explicitly pass it by pointer using the `&` keyword.
 *
 * @warning
 * Please note that passing a function by reference or by pointer
 * will prevent the compiler from inlining it (you may have a chance
 * by reference).
 *
 * @warning
 * To avoid this possible performance penalty, you should pass the function
 * through a lambda :
 * @snippet exampleFunctorHolder.cpp Holding a function through a lambda
 * @snippet exampleFunctorHolder.cpp Holding a templated function through a lambda
 * thus giving the compiler free hands to inline it.
 *
 * @warning
 * Even better: if you have enable C++14 support, you can use generic lambda
 * so that you don't have to specify any type, even for templated functions
 * (the type is resolved at the effective call):
 * @snippet exampleFunctorHolder.cpp Holding a templated function through a C++14 lambda
 * Enjoy the genericity !
 *
 * @subsection holding_a_functor Holding a functor
 *
 * If you want to refer to a functor, you can pass it by (lvalue) reference:
 * @snippet exampleFunctorHolder.cpp Functor
 * @snippet exampleFunctorHolder.cpp Holding a functor by lvalue ref
 *
 * You wan also inline the construction of the functor directly in the
 * @ref holdFunctor call:
 * @snippet exampleFunctorHolder.cpp Holding a functor by rvalue ref
 * or, to increase code readability, you can first construct the functor
 * and then transfer its ownership to @ref holdFunctor by using the move
 * semantic:
 * @snippet exampleFunctorHolder.cpp Holding a functor by moving it
 *
 * @remark
 * Moving the functor to @ref holdFunctor is also a way to increase its
 * lifetime, for example when returning a @ref FunctorHolder that depends
 * on a local functor.
 * See also the section @ref held_object_lifetime.
 *
 * @subsection holding_a_lambda Holding a lambda
 * Without surprise, holding a lambda works the same way:
 * @snippet exampleFunctorHolder.cpp Holding a lambda
 *
 * @subsection holding_something_else Holding something else
 * @ref FunctorHolder should be able to hold any callable object.
 * However, as warned before, if you are concerned by performance,
 * you should avoid holding a function by reference or pointer
 * or, even worse, holding a `std::function` that is more or less a pointer
 * with an additional cost due to the polymorphism.
 *
 * @subsection held_object_lifetime Held object lifetime
 * When passing a functor to @ref holdFunctor by lvalue reference
 * (ie the functor has a name), the functor lifetime must exceed the
 * @ref FunctorHolder lifetime.
 *
 * Otherwise, consider constructing the functor directly during the
 * @ref holdFunctor call or transfer its ownership by using the move semantic.
 * See the examples in section @ref holding_a_functor.
 *
 * @subsection auto_holdfunctor Why using the `auto` keyword ?
 * Since the exact storage type used in @ref FunctorHolder is choosen by
 * @ref holdFunctor depending on the passing mode of the callable object,
 * it is not easily possible to known the template parameters of
 * @ref FunctorHolder.
 *
 * Thus, it is recommended to use the `auto` keyword as the type placeholder
 * for any instance of @ref FunctorHolder.
 *
 * See also the section about @ref storing_functorholder.
 *
 *
 * @section calling_the_held_object Calling the held object
 *
 * Calling the held object is done naturally by using the `operator()`, like
 * in the previous examples:
 * @snippet exampleFunctorHolder.cpp Holding a functor by rvalue ref
 *
 * @subsection parameters_return_types What about the parameters and return value types ?
 *
 * You may have notice that we never have to specify the types of the
 * parameters used when calling the held object, neither the type of the
 * returned object.
 *
 * The trick behind this is the use of variadic templates and perfect forwarding
 * so that the call of the held object is transparent for @ref FunctorHolder.
 * The returned value type is also automatically deduced.
 *
 * @subsection functor_arity What about the callable object arity ?
 * The use of variadic templates for the `operator ()` allows holding
 * a callable object of any arity:
 * @snippet exampleFunctorHolder.cpp Holding a non-unary lambda
 *
 *
 * @section copy_and_assignment Copying and assigning a FunctorHolder
 *
 * A @ref FunctorHolder instance is copyable, movable and assignable,
 * thus making it a @ref boost::Assignable model and any other concept
 * that trivially inherit from it (e.g. @ref concepts::CUnaryFunctor).
 *
 * @warning
 * When copying or assigning a @ref FunctorHolder, the two involved instances
 * will afterward both refer to the same callable object (i.e. the held object
 * is not copied).
 * For example, modifying a functor attribute after the copy will modify the
 * result of all original and copied instance of @ref FunctorHolder that hold
 * it:
 * @snippet exampleFunctorHolder.cpp Copying a functor by lvalue ref
 *
 * @warning
 * It also the case when passing the object by rvalue reference to
 * @ref holdFunctor since it moves it to a dynamically allocated memory
 * place, managed by `std::shared_ptr`.
 * Copying or assigning the resulting @ref FunctorHolder is like copying
 * or assigning the underlying `std::shared_ptr`:
 * @snippet exampleFunctorHolder.cpp Copying a lambda by rvalue ref
 *
 * @note
 * A counterpart of this design it that @ref FunctorHolder instances are
 * lightweight.
 *
 *
 * @section type_of_functorholder The type of a FunctorHolder instance
 *
 * @note
 * Note that the above topics are not specific to @ref FunctorHolder.
 * They may be useful in many cases where you can't or don't want to
 * guess the result type of an expression.
 *
 *
 * @subsection storing_functorholder Storing a FunctorHolder
 * As explained before (see @ref auto_holdfunctor), you cannot easily guess
 * the result type of a @ref holdFunctor call.
 * Moreover, it becomes impossible when passing a lambda in an inline way.
 *
 * Thus, it is recommended to use the `auto` keyword as the type placeholder
 * for any instance of @ref FunctorHolder :
 * @snippet exampleFunctorHolder.cpp Storing a FunctorHolder
 *
 *
 * @subsection passing_functorholder Passing a FunctorHolder as a parameter
 *
 * However, when passing a @ref FunctorHolder, espacially to a class constructor, you
 * may still need to know the @ref FunctorHolder exact type (including
 * the template parameters).
 *
 * In those cases, a solution is to first store the @ref FunctorHolder and then
 * deduce its type by using the `decltype` keyword:
 * @snippet exampleFunctorHolder.cpp Passing a FunctorHolder
 *
 * To ease such usage, you may want to search if there exist a helper (or factory)
 * for that class (see also @ref creating_a_helper).
 *
 *
 * @subsection returning_functorholder Returning a FunctorHolder
 * The most tricky part begins when you need a function to return a
 * @ref FunctorHolder.
 *
 * The problem comes from the fact that up to C++11 standard, you need to
 * somehow specify the function's return type.
 * In C++11, you can slightly delay this type specification using the trailing
 * return syntax but the type still needs to be known in the signature.
 * Basically, you need to duplicate the line of code that generates the
 * @ref FunctorHolder (optionaly using the function's parameters) into the
 * function signature and deduce its type using `decltype`:
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder in caller
 *
 * If it is more easier to get the return type using the actual parameters,
 * you can use the trailing return syntax:
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder using trailing return
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder using trailing return in caller
 *
 * @note
 * Note that you don't have to put the exact same expression in the trailing
 * return type deduction and in the actual return. Like in the previous
 * snippet, you can simply use another expression you know the result type will
 * be the same as the actual return expression.
 *
 * @note
 * Going further, if writting such simplier expression is difficult, you can
 * use `std::declval` function that construct a fake instance of any given type:
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder using trailing return and declval
 *
 *
 * Starting with C++14 standard, you can simply use the `auto` keyword as
 * a return type and the compiler should deduce the actual type from the
 * `return` statements of the function:
 * @snippet exampleFunctorHolder.cpp Returning a FunctorHolder using auto in C++14
 *
 *
 * @subsection creating_a_helper Creating a helper
 *
 * Usage of @ref FunctorHolder (as other kind of objects whose type is
 * difficult to guess) can be simplified by adding helpers (or factories)
 * to classes whose template parameters depend on such objects.
 *
 * A helper is only a templated function that benefits from the auto deduction
 * of template parameters of a function in order to deduce the appropriate
 * class type:
 * @snippet exampleFunctorHolder.cpp Factory of Binarizer
 * @snippet exampleFunctorHolder.cpp Using the Binarizer factory
 *
 * @note
 * Starting with C++17, these helpers can be replaced by deduction guides that
 * are custom rules for deducing class template parameters from a direct call
 * to the constructor (without specifying the deductible template parameters):
 * @snippet exampleFunctorHolder.cpp Binarizer deduction guide in C++17
 *
 *
 * For more complex classes, like @ref DGtal::functors::PointFunctorPredicate
 * that use @ref DGtal::ConstAlias in the constructor parameters, you cannot
 * simply use @ref DGtal::ConstAlias in the factory and hope that the compiler
 * will deduce the aliases type:
 * @snippet exampleFunctorHolder.cpp Wrong factory of PointFunctorPredicate
 *
 * The problem here is that implicit conversions are ignored during the
 * template deduction step.
 * In this case, the first solution is to remove the @ref DGtal::ConstAlias
 * from the helper signature:
 * @snippet exampleFunctorHolder.cpp Factory of PointFunctorPredicate
 * @snippet exampleFunctorHolder.cpp Using the PointFunctorPredicate factory
 *
 * Another problem arises here: the constructor of
 * @ref DGtal::functors::PointFunctorPredicate will get as parameters only
 * left-value references because the parameters have a name in the factory.
 * Thus, you might miss some optimizations for right-value references.
 *
 * In order to make a factory that doesn't change the parameters type, you
 * must use forwarding references (by using `&&` references together with
 * template paremeter deduction, also known as universal references)
 * and perfect forwarding using `std::forward`:
 * @snippet exampleFunctorHolder.cpp Factory of PointFunctorPredicate using perfect forwarding
 * Note the use of `std::decay` because the template parameter will be deduced
 * with an included reference specification that you don't want to be part of
 * the returned class specification (`std::decay` removes reference and
 * constness from a given type).
 *
 *
 * @section marking_cunaryfunctor Making a C(Unary)Functor model based on FunctorHolder
 *
 * In the following sections, we will explain how to create new DGtal functor
 * models using @ref FunctorHolder as an internal storage in order to accept
 * any kind of callable objects (lambda included).
 *
 * You may want to add such classes because some concepts derived from
 * @ref DGtal::concepts::CUnaryFunctor may need additional data or typedef,
 * like @ref DGtal::concepts::CPointFunctor.
 *
 * @subsection tutorial_cpointfunctor A simple CUnaryFunctor model with additional typedef
 *
 *
 * @see ???
 * @see holdFunctor
 */
template <
  typename FunctorStorage,
  bool NeedDereference
>
class FunctorHolder
{
  // ------------------------- Private Datas --------------------------------
private:

  FunctorStorage myFunctor; ///< The callable object.

  // ----------------------- Standard services ------------------------------
public:

  /** Constructor
   *
   * @tparam  Function  The type of the callable object (auto-deduced).
   * @param   fn        The callable object.
   */
  template <
    typename Function,
    // SFINAE trick to disable this constructor in a copy/move construction context.
    typename std::enable_if<!std::is_base_of<FunctorHolder, typename std::decay<Function>::type>::value, int>::type = 0
  >
  explicit FunctorHolder(Function && fn)
      : myFunctor(std::forward<Function>(fn))
  {
  }

  // ----------------------- Interface --------------------------------------
public:

  /** Invokes the stored callable object in a constant context.
   *
   * @tparam  T     Arguments's types.
   * @param   args  The arguments.
   */
  template <typename... T>
  inline auto operator() (T &&... args) const
      -> decltype(Invoker<NeedDereference>::apply(this->myFunctor, std::forward<T>(args)...))
    {
      return Invoker<NeedDereference>::apply(myFunctor, std::forward<T>(args)...);
    }

  /** Invokes the stored callable object in a mutable context.
   *
   * @tparam  T     Arguments's types.
   * @param   args  The arguments.
   */
  template <typename... T>
  inline auto operator() (T &&... args)
      -> decltype(Invoker<NeedDereference>::apply(this->myFunctor, std::forward<T>(args)...))
    {
      return Invoker<NeedDereference>::apply(myFunctor, std::forward<T>(args)...);
    }

  /**
   * Writes/Displays the object on an output stream.
   * @param out the output stream where the object is written.
   */
  inline
  void selfDisplay ( std::ostream & out ) const
    {
      out << "[FunctorHolder]";
      if ( SharedPtrTrait::apply(myFunctor) )
        out << " using std::shared_ptr storage (rvalue)";
      else if ( ReferenceWrapperTrait::apply(myFunctor) )
        out << " using std::reference_wrapper storage (lvalue)";
      else
        out << " using custom storage";
    }

  /**
   * Checks the validity/consistency of the object.
   * @return 'true' if the object is valid, 'false' otherwise.
   */
  inline constexpr
  bool isValid() const
    {
      return true;
    }
}; // End of class FunctorHolder

/**
 * Overloads 'operator<<' for displaying objects of class 'XXX'.
 * @param out the output stream where the object is written.
 * @param object the object of class 'XXX' to write.
 * @return the output stream after the writing.
 */
template <typename FunctorStorage, bool NeedDereference>
std::ostream&
operator<< ( std::ostream & out, const FunctorHolder<FunctorStorage, NeedDereference> & object )
{
  object.selfDisplay( out );
  return out;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation details
namespace {

  /// Overload that stores the object in a std::reference_wrapper when given object is a lvalue reference.
  template <typename Function>
  inline auto holdFunctorImpl(Function && fn, std::true_type)
      -> FunctorHolder<decltype(std::ref(std::forward<Function>(fn))), false>
    {
      return FunctorHolder<decltype(std::ref(std::forward<Function>(fn))), false>{ std::forward<Function>(fn) };
    }

  /// Overload that stores the object in a std::shared_ptr when given object isn't a lvalue reference.
  template <typename Function>
  inline auto holdFunctorImpl(Function && fn, std::false_type)
      -> FunctorHolder<std::shared_ptr<Function>, true>
  {
      return FunctorHolder<std::shared_ptr<Function>, true>{ std::make_shared<Function>(std::forward<Function>(fn)) };
  }

} // anonymous namespace


//////////////////////////////////////////////////////////////////////////////
// FunctorHolder construction helper
/** @brief Hold any callable object (function, functor, lambda, ...) as a C(Unary)Functor model.
 *
 * @tparam  Function  The callable object type (auto-deduced, do not specify it)
 * @param   fn        The callable object to be holded.
 * @return  a FunctorHolder instance with appropriate storage type depending on the given object.
 *
 * @see FunctorHolder documentation for more informations
 */
template <typename Function>
inline auto holdFunctor(Function && fn)
  -> decltype(holdFunctorImpl(std::forward<Function>(fn), typename std::is_lvalue_reference<Function>{})) // Can we please pass to C++14 ?
{
  return holdFunctorImpl(std::forward<Function>(fn), typename std::is_lvalue_reference<Function>{});
}

} // namespace DGtal

#endif // !defined FunctorHolder_h

#undef FunctorHolder_RECURSES
#endif // else defined(FunctorHolder_RECURSES)
