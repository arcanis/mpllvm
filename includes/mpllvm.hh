#include <llvm/ADT/ArrayRef.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Type.h>

#include <vector>

namespace mpllvm
{

    namespace internal
    {

        // Forward declaration of the type resolver

        template < typename Type >
        struct TypeResolver;

        template < typename... Types >
        struct TypesResolver;

        // Template functions getting types of a variadic template list

        template < typename T, typename... R >
        struct TypesResolver< T, R... > {
            static std::vector< llvm::Type * > && list( llvm::LLVMContext & llvmContext, int position = 0 )
            {
                std::vector< llvm::Type * > && typeList = mpllvm::internal::TypesResolver< R... >::list( llvmContext, position + 1 );
                typeList[ position ] = mpllvm::internal::TypeResolver< T >::get( llvmContext );
                return typeList;
            }
        };

        template < >
        struct TypesResolver< > {
            static std::vector < llvm::Type * > list( llvm::LLVMContext &, int count = 0 )
            {
                std::vector< llvm::Type * > vector;
                vector.resize( count );
                return vector;
            }
        };

        // Integer type resolver (code factorizing)

        template < typename T >
        struct IntegerTypeResolver {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::IntegerType::get( llvmContext, sizeof( T ) * 8 );
            }
        };

        template < > struct TypeResolver< signed char > : public IntegerTypeResolver< signed char > { };
        template < > struct TypeResolver< unsigned char > : public IntegerTypeResolver< unsigned char > { };
        template < > struct TypeResolver< signed short > : public IntegerTypeResolver< signed short > { };
        template < > struct TypeResolver< unsigned short > : public IntegerTypeResolver< unsigned short > { };
        template < > struct TypeResolver< signed int > : public IntegerTypeResolver< signed int > { };
        template < > struct TypeResolver< unsigned int > : public IntegerTypeResolver< unsigned int > { };
        template < > struct TypeResolver< signed long > : public IntegerTypeResolver< signed long > { };
        template < > struct TypeResolver< unsigned long > : public IntegerTypeResolver< unsigned long > { };
        template < > struct TypeResolver< signed long long > : public IntegerTypeResolver< signed long long > { };
        template < > struct TypeResolver< unsigned long long > : public IntegerTypeResolver< unsigned long long > { };

        // Basic specializations

        template < >
        struct TypeResolver< void > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::Type::getVoidTy( llvmContext );
            }
        };

        template < >
        struct TypeResolver< double > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::Type::getDoubleTy( llvmContext );
            }
        };

        template < >
        struct TypeResolver< float > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::Type::getFloatTy( llvmContext );
            }
        };

        // Complex specializations

        template < typename Return, typename... Args >
        struct TypeResolver< Return (*)( Args... ) > {
            static llvm::FunctionType * get( llvm::LLVMContext & llvmContext ) {
                return llvm::FunctionType::get( mpllvm::internal::TypeResolver< Return >::get( llvmContext ), llvm::ArrayRef< llvm::Type * >( mpllvm::internal::TypesResolver< Args... >::list( llvmContext ) ), false );
            }
        };

        template < typename Return, typename... Args >
        struct TypeResolver< Return (*)( Args... ... ) > {
            static llvm::FunctionType * get( llvm::LLVMContext & llvmContext ) {
                return llvm::FunctionType::get( mpllvm::internal::TypeResolver< Return >::get( llvmContext ), llvm::ArrayRef< llvm::Type * >( mpllvm::internal::TypesResolver< Args... >::list( llvmContext ) ), true );
            }
        };

        template < typename Type >
        struct TypeResolver< Type * > {
            static llvm::PointerType * get( llvm::LLVMContext & llvmContext ) {
                return llvm::PointerType::get( mpllvm::internal::TypeResolver< Type >::get( llvmContext ), 0 );
            }
        };

    }

    // Returns the LLVM type corresponding to the template parameter

    template < typename T >
    auto get( llvm::LLVMContext & llvmContext ) -> decltype( mpllvm::internal::TypeResolver< T >::get( llvmContext ) ) {
        return mpllvm::internal::TypeResolver< T >::get( llvmContext );
    }

    // Try to deduce the LLVM type based on the parameter variable

    template < typename T >
    auto deduce( llvm::LLVMContext & llvmContext, T const & t ) -> decltype( mpllvm::get< T >( llvmContext ) ) {
        return mpllvm::get< T >( llvmContext );
    }

    template < typename... Types >
    llvm::StructType * craft( llvm::LLVMContext & llvmContext, bool isPacked = false ) {
        return llvm::StructType::get( llvmContext, mpllvm::internal::TypesResolver< Types... >::list( llvmContext ) );
    }

}
