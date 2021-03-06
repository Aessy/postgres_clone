// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_CONNECTION_POOL_HPP
#define TAOCPP_INCLUDE_POSTGRES_CONNECTION_POOL_HPP

#include <string>
#include <memory>
#include <utility>

#include <tao/utility/pool.hpp>

#include <tao/postgres/connection.hpp>
#include <tao/postgres/result.hpp>

namespace tao
{
  namespace postgres
  {
    class connection_pool
      : public utility::pool< postgres::connection >
    {
    private:
      std::string connection_info_;
      std::unordered_map<std::string, std::string> prepared_statements;

      std::unique_ptr< postgres::connection > v_create() const override;
      bool v_is_valid( postgres::connection& c ) const override;

    public:
      static std::shared_ptr< connection_pool > create( const std::string& connection_info );

    private:
      // pass-key idiom
      class private_key
      {
        private_key() = default;
        friend std::shared_ptr< connection_pool > connection_pool::create( const std::string& connect_info );
      };

    public:
      connection_pool( const private_key&, const std::string& connection_info );

      void prepare(std::string const& key, std::string const& statement)
      {
          prepared_statements[key] = statement;
      }

      std::shared_ptr< postgres::connection > connection()
      {
        return this->get();
      }

      template< typename... Ts >
      result execute( Ts&&... ts )
      {
        return this->connection()->direct()->execute( std::forward< Ts >( ts )... );
      }
    };
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_CONNECTION_POOL_HPP
