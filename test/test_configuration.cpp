/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * SPDX-FileCopyrightText: 2026 René Moll
 * SPDX-License-Identifier: MPL-2.0
 */

#include "example/configuration.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/trompeloeil.hpp>

class FakeConfigurationStorage : public trompeloeil::mock_interface<IConfigurationStorage>
{
public:
	// MAKE_MOCK1(load, auto(Type bank)->std::expected<Configuration, Error>, override);
	// MAKE_MOCK2(save, auto(Type bank, const Configuration& config)->std::expected<void, Error>, override);
	IMPLEMENT_MOCK1(load);
	IMPLEMENT_MOCK2(save);

	// FakeConfigurationStorage()
	// {
	// 	// TODO: initialize with different configurations
	// 	m_active = Configuration{};
	// 	m_factory_default = Configuration{};
	// }

	// [[nodiscard]] std::expected<Configuration, Error> load(Type bank) override
	// {
	// 	// TODO: simulate different scenarios based on the return type

	// 	switch (bank) {
	// 		case Type::Active:
	// 			return m_active;
	// 		case Type::FactoryDefault:
	// 			return m_factory_default;
	// 	}

	// 	return std::unexpected(Error::StorageFailure);
	// }

	// [[nodiscard]] std::expected<void, Error> save(Type bank, const Configuration& config) override
	// {
	// 	// TODO: simulate different scenarios based on the return type

	// 	switch (bank) {
	// 		case Type::Active:
	// 			m_active = config;
	// 			return {};
	// 		case Type::FactoryDefault:
	// 			m_factory_default = config;
	// 			return {};
	// 	}

	// 	return std::unexpected(Error::StorageFailure);
	// }

	// Configuration m_active;
	// Configuration m_factory_default;
};

SCENARIO("Configuration: constructors")
{
	GIVEN("a ConfigurationStore")
	{
		WHEN("constructed")
		{
			auto storage = std::make_unique<FakeConfigurationStorage>();
			ConfigurationStore store(std::move(storage));

			THEN("active and staging return the default-initialized configuration")
			{
				REQUIRE(store.active() == Configuration{});
				REQUIRE(store.staging() == Configuration{});
			}
		}
	}
}

SCENARIO("Configuration: assignment")
{
	static_assert(!std::is_copy_assignable_v<ConfigurationStore>);
	static_assert(std::is_move_assignable_v<ConfigurationStore>);
}

SCENARIO("Configuration: observers")
{
	using StorageType = IConfigurationStorage::Type;
	using LoadResult = ConfigurationStore::LoadResult;
	using StorageError = IConfigurationStorage::Error;

	const Configuration active{.activeHigh = false, .divider = 2, .gain = 2.0f};
	const Configuration backup{.activeHigh = true, .divider = 1, .gain = 1.0f};

	GIVEN("a ConfigurationStore with valid active configuration in persistent storage")
	{
		auto storage = std::make_unique<FakeConfigurationStorage>();
		REQUIRE_CALL(*storage.get(), load(StorageType::Active)).RETURN(active);

		ConfigurationStore store(std::move(storage));

		WHEN("init")
		{
			CHECK(store.init() == LoadResult::Ok);

			THEN("active and staging are loaded with the active configuration")
			{
				CHECK(store.active() == active);
				CHECK(store.staging() == active);
			}
		}
	}

	GIVEN("a ConfigurationStore with invalid active configuration in persistent storage")
	{
		auto storage = std::make_unique<FakeConfigurationStorage>();
		auto storage_return_value =
			GENERATE(StorageError::NotFound, StorageError::StorageFailure, StorageError::ChecksumFailed,
					 StorageError::InvalidFormat, StorageError::VersionCheckFailed);
		REQUIRE_CALL(*storage.get(), load(StorageType::Active)).RETURN(std::unexpected(storage_return_value));

		const auto expected_load_result = [&] {
			switch (storage_return_value) {
				case StorageError::NotFound:
					return LoadResult::NotFound;
				case StorageError::StorageFailure:
					return LoadResult::StorageFailure;
				case StorageError::ChecksumFailed:
				case StorageError::InvalidFormat:
				case StorageError::VersionCheckFailed:
					return LoadResult::CorruptedConfiguration;
			}
			return LoadResult::NotFound;
		}();

		AND_GIVEN("a valid backup configuration in persistent storage")
		{
			REQUIRE_CALL(*storage.get(), load(StorageType::FactoryDefault)).RETURN(backup);
			ConfigurationStore store(std::move(storage));

			WHEN("init")
			{
				CHECK(store.init() == expected_load_result);

				THEN("active and staging are loaded with the backup configuration")
				{
					CHECK(store.active() == backup);
					CHECK(store.staging() == backup);
				}
			}
		}

		AND_GIVEN("an invalid backup configuration in persistent storage")
		{
			REQUIRE_CALL(*storage.get(), load(StorageType::FactoryDefault))
				.RETURN(std::unexpected(StorageError::StorageFailure));
			ConfigurationStore store(std::move(storage));

			WHEN("init")
			{
				CHECK(store.init() == expected_load_result);

				THEN("active and staging are loaded with the default-constructed configuration")
				{
					CHECK(store.active() == Configuration{});
					CHECK(store.staging() == Configuration{});
				}
			}
		}
	}
}

SCENARIO("Configuration: modifiers")
{
	using StorageType = IConfigurationStorage::Type;
	using LoadResult = ConfigurationStore::LoadResult;
	using StorageError = IConfigurationStorage::Error;
	using StoreResult = ConfigurationStore::StoreResult;

	const Configuration active{.activeHigh = false, .divider = 2, .gain = 2.0f};
	const Configuration backup{.activeHigh = true, .divider = 1, .gain = 1.0f};

	GIVEN("a ConfigurationStore with a modified staging configuration")
	{
		auto storage = std::make_unique<FakeConfigurationStorage>();
		ALLOW_CALL(*storage.get(), load(StorageType::Active)).RETURN(active);
		ALLOW_CALL(*storage.get(), load(StorageType::FactoryDefault)).RETURN(backup);
		ALLOW_CALL(*storage.get(), save(StorageType::Active, trompeloeil::_))
			.RETURN(std::expected<void, StorageError>{});

		ConfigurationStore store(std::move(storage));
		CHECK(store.init() == LoadResult::Ok);

		store.staging().divider = 3;

		THEN("the active configuration remains unchanged")
		{
			CHECK(store.active() == active);
		}

		WHEN("committing the staging configuration")
		{
			CHECK(store.commit() == StoreResult::Ok);

			THEN("the active configuration is updated")
			{
				CHECK(store.staging().divider == 3);
				CHECK(store.active() == store.staging());
			}
		}

		WHEN("re-initializing")
		{
			CHECK(store.init() == LoadResult::Ok);

			THEN("the active and staging configurations are reloaded from storage")
			{
				CHECK(store.active() == active);
				CHECK(store.staging() == active);
			}
		}

		WHEN("loading the factory default configuration")
		{
			CHECK(store.load_factory_default() == LoadResult::Ok);

			THEN("the active and staging configurations hold the factory default configuration")
			{
				CHECK(store.active() == backup);
				CHECK(store.staging() == backup);
			}
		}
	}

	GIVEN("a ConfigurationStore with a modified staging configuration")
	{
		auto storage = std::make_unique<FakeConfigurationStorage>();
		REQUIRE_CALL(*storage.get(), load(StorageType::Active)).RETURN(active);
		REQUIRE_CALL(*storage.get(), save(StorageType::Active, trompeloeil::_))
			.RETURN(std::unexpected(StorageError::StorageFailure));

		ConfigurationStore store(std::move(storage));
		CHECK(store.init() == LoadResult::Ok);

		store.staging().divider = 3;

		WHEN("commit fails")
		{
			CHECK(store.commit() == StoreResult::FailedToWrite);

			THEN("the active configuration remains unchanged")
			{
				CHECK(store.active() == active);
			}

			THEN("the staging configuration remains unchanged")
			{
				CHECK(store.staging().divider == 3);
			}
		}
	}
}
