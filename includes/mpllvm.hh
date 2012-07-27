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
            static std::vector< llvm::Type * > list( llvm::LLVMContext & llvmContext, int position = 0 )
            {
                std::vector< llvm::Type * > && typeList = mpllvm::internal::TypesResolver< R... >::list( llvmContext, position + 1 );
                typeList[ position ] = mpllvm::internal::TypeResolver< T >::get( llvmContext );
                return typeList;
            }
        };

        template < >
        struct TypesResolver< > {
            static std::vector < llvm::Type * > list( llvm::LLVMContext &, int = 0 )
            {
                return std::vector< llvm::Type * >( );
            }
        };

        // Basic specializations

        template < >
        struct TypeResolver< void > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::Type::getVoidTy( llvmContext );
            }
        };

        template < >
        struct TypeResolver< int > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::IntegerType::get( llvmContext, sizeof( int ) * 8 );
            }
        };

        template < >
        struct TypeResolver< unsigned int > {
            static llvm::Type * get( llvm::LLVMContext & llvmContext ) {
                return llvm::IntegerType::get( llvmContext, sizeof( int ) * 8 );
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
    llvm::Type * get( llvm::LLVMContext & llvmContext ) {
        return mpllvm::internal::TypeResolver< T >::get( llvmContext );
    }

    // Try to deduce the LLVM type based on the parameter variable

    template < typename T >
    llvm::Type * deduce( llvm::LLVMContext & llvmContext, T const & t ) {
        return mpllvm::get< T >( llvmContext );
    }

    template < typename... Types >
    llvm::StructType * craft( llvm::LLVMContext & llvmContext, bool isPacked = false ) {
        return llvm::StructType::get( llvmContext, mpllvm::internal::TypesResolver< Types... >::list( llvmContext ) );
    }

}
