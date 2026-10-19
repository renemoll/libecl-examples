#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cinttypes>
#include <cmath>
#include <expected>
#include <memory>

struct Configuration
{
	bool activeHigh;
	// time_ms fade_on_duration;
	// time_ms fade_off_duration;
	// time_ms blink_period;

	// TODO: remove
	int divider;  // NOLINT(misc-unused-parameters)
	float gain;	  // NOLINT(misc-unused-parameters)
};

bool operator==(const Configuration& lhs, const Configuration& rhs);
bool operator!=(const Configuration& lhs, const Configuration& rhs);

// TODO: keys are nice.. but values can differ in types/sizes etc..
//       next version uses a key-value approach?
// enum class ConfigurationKeys
// {
// 	ActiveHigh,
// 	Divider,
// 	Gain
// };

/*
 * \brief Interface for configuration storage.
 *
 * Expected to:
 * - serialize configuration data for storage.
 * - deserialize configuration data from storage.
 * - ensure data integrity through checksums or other mechanisms.
 * - proper storage of configuration data.
 */
class IConfigurationStorage
{
public:
	enum class Error
	{
		NotFound,			 //!< No configuration found.
		InvalidFormat,		 //!< The configuration format is invalid.
		ChecksumFailed,		 //!< The configuration checksum verification failed.
		VersionCheckFailed,	 //!< The configuration version check failed.
		StorageFailure		 //!< Unable to access the storage.
	};

	enum class Type
	{
		FactoryDefault,
		Active,
	};

	virtual ~IConfigurationStorage() = default;

	/**
	 * \brief Load the configuration from the specified bank.
	 *
	 * \param bank The bank from which to load the configuration.
	 * \return An expected containing the configuration if successful, or an Error indicating failure.
	 */
	[[nodiscard]] virtual std::expected<Configuration, Error> load(Type bank) = 0;

	/**
	 * \brief Save the configuration to the specified bank.
	 *
	 * Once the function returns successfully, the configuration is considered saved and will persist in the specified
	 * bank.
	 *
	 * \param bank The bank to which to save the configuration.
	 * \param config The configuration to save.
	 * \return An expected indicating success or an Error if the operation failed.
	 */
	[[nodiscard]] virtual std::expected<void, Error> save(Type bank, const Configuration& config) = 0;
};

/*!
 * \brief Holds the current configuration.
 * \class ConfigurationStore
 *
 * \c ConfigurationStore holds the active configuration and handles configuration updates. One can always access the
 * active configuration through the \c active_set() method. The active set cannot be manipulated directly; one must use
 * the staging set to make changes.
 *
 * Upon initialization, the active configuration is loaded from storage and copied into the staging set. If loading
 * the active configuration fails, the factory defaults are loaded instead. If that also fails, the \c Configuration
 * structure will be default initialized.
 *
 * The staging set can be manipulated through the \c staging_set() method. Once complete, changes have to be committed
 * using the \c commit() method before taking effect. A commit shall first save the staging set to non-volatile
 * storage. Only after the commit succeeds will the active set be with the contents of the staging set. If the commit
 * fails, the active set remains unchanged.
 *
 * At any point, the backup configuration can be loaded into both the staging and active sets by calling
 * \c load_factory_default(). Afterwards, the user must call \c commit() to permanently save the factory defaults as
 * active configuration.
 *
 * \note The factory defaults cannot be modified through the \c ConfigurationStore.
 *
 * Assumptions:
 * - Configuration validity is to be checked by other means.
 * - Thread-safety is the responsibility of the caller.
 */
class ConfigurationStore
{
public:
	// TODO: Result vs expected
	enum class LoadResult  // NOLINT(performance-enum-size)
	{
		Ok,						 //!< Operation succeeded.
		NotFound,				 //!< Configuration not found.
		CorruptedConfiguration,	 //!< Configuration is corrupted.
		StorageFailure			 //!< Unable to access the storage.
	};

	enum class StoreResult	// NOLINT(performance-enum-size)
	{
		Ok,				//!< Operation succeeded.
		FailedToWrite,	//!< Writing to storage failed.
		FailedToVerify	//!< Verification of the written configuration failed.
	};

	explicit ConfigurationStore(std::unique_ptr<IConfigurationStorage> storage);
	ConfigurationStore(const ConfigurationStore&) = delete;
	ConfigurationStore(ConfigurationStore&&) = default;
	~ConfigurationStore() = default;

	ConfigurationStore& operator=(const ConfigurationStore&) = delete;
	ConfigurationStore& operator=(ConfigurationStore&&) = default;

	[[nodiscard]] LoadResult init();
	[[nodiscard]] LoadResult load_factory_default();

	[[nodiscard]] const Configuration& active() const
	{
		return m_active;
	}

	[[nodiscard]] Configuration& staging()
	{
		return m_staging;
	}

	[[nodiscard]] StoreResult commit();

private:
	std::unique_ptr<IConfigurationStorage> m_storage;
	Configuration m_active;
	Configuration m_staging;
};

#endif	// CONFIGURATION_H
