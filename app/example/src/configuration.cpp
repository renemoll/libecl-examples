/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * SPDX-FileCopyrightText: 2026 René Moll
 * SPDX-License-Identifier: MPL-2.0
 */

#include "example/configuration.hpp"

bool operator==(const Configuration& lhs, const Configuration& rhs)
{
	return lhs.activeHigh == rhs.activeHigh && lhs.divider == rhs.divider && std::abs(lhs.gain - rhs.gain) < 1e-6F;
}

bool operator!=(const Configuration& lhs, const Configuration& rhs)
{
	return !(lhs == rhs);
}

ConfigurationStore::ConfigurationStore(std::unique_ptr<IConfigurationStorage> storage)
	: m_storage(std::move(storage))
{
}

ConfigurationStore::LoadResult ConfigurationStore::init()
{
	const auto result_active = m_storage->load(IConfigurationStorage::Type::Active);
	if (result_active) {
		m_staging = *result_active;
		m_active = m_staging;
		return LoadResult::Ok;
	}

	const auto result_default = load_factory_default();
	std::ignore = result_default;

	switch (result_active.error()) {
		case IConfigurationStorage::Error::NotFound:
			return LoadResult::NotFound;

		case IConfigurationStorage::Error::InvalidFormat:
		case IConfigurationStorage::Error::VersionCheckFailed:
		case IConfigurationStorage::Error::ChecksumFailed:
			return LoadResult::CorruptedConfiguration;

		case IConfigurationStorage::Error::StorageFailure:
			return LoadResult::StorageFailure;
	}

	return LoadResult::NotFound;
}

ConfigurationStore::LoadResult ConfigurationStore::load_factory_default()
{
	const auto result = m_storage->load(IConfigurationStorage::Type::FactoryDefault);
	if (result) {
		m_staging = *result;
		m_active = m_staging;
		return LoadResult::Ok;
	} else {
		m_staging = Configuration{};
		m_active = m_staging;

		switch (result.error()) {
			case IConfigurationStorage::Error::NotFound:
				return LoadResult::NotFound;

			case IConfigurationStorage::Error::InvalidFormat:
			case IConfigurationStorage::Error::VersionCheckFailed:
			case IConfigurationStorage::Error::ChecksumFailed:
				return LoadResult::CorruptedConfiguration;

			case IConfigurationStorage::Error::StorageFailure:
				return LoadResult::StorageFailure;
		}
	}

	return LoadResult::NotFound;
}

ConfigurationStore::StoreResult ConfigurationStore::commit()
{
	if (m_active == m_staging) {
		return StoreResult::Ok;
	}

	const auto result = m_storage->save(IConfigurationStorage::Type::Active, m_staging);
	if (!result) {
		switch (result.error()) {
			case IConfigurationStorage::Error::NotFound:
			case IConfigurationStorage::Error::InvalidFormat:
			case IConfigurationStorage::Error::VersionCheckFailed:
			case IConfigurationStorage::Error::StorageFailure:
				return StoreResult::FailedToWrite;

			case IConfigurationStorage::Error::ChecksumFailed:
				return StoreResult::FailedToVerify;
		}
	}

	m_active = m_staging;
	return StoreResult::Ok;
}
