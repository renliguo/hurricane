# The name of the project component
NAME := lib_cryptoauthlib

$(NAME)_PREBUILT_LIBRARIES := lib_cryptoauthlib.a

$(NAME)_GLOBAL_INCLUDES := lib \
						   lib/crypto \
						   lib/host \
						   lib/hal \
					       lib/basic \
					       lib/atcacert \
					       test \
					       . \
					       silabs

$(NAME)_DEFINES := ATCA_HAL_I2C

$(NAME)_CFLAGS := -Wno-unused-function \
				  -Wno-unused-variable \
				  -fno-strict-aliasing
				  
$(NAME)_GLOBAL_LDFLAGS += -Wl,--wrap=putchar

# uncomment to build library
# $(NAME)_SOURCES :=  lib/crypto/atca_crypto_sw_sha1.c \
# 					lib/crypto/hashes/sha1_routines.c \
# 					lib/crypto/hashes/sha2_routines.c \
# 					lib/crypto/atca_crypto_sw_sha2.c \
# 					lib/crypto/atca_crypto_sw_ecdsa.c \
# 					lib/crypto/atca_crypto_sw_rand.c \
# 					lib/atca_cfgs.c \
# 					lib/host/atca_host.c \
# 					lib/hal/kit_protocol.c \
# 					lib/hal/atca_hal.c \
# 					silabs/hal_gos_i2c.c \
# 					silabs/hal_gos_timer.c \
# 					lib/basic/atca_basic_aes_cmac.c \
# 					lib/basic/atca_basic_nonce.c \
# 					lib/basic/atca_basic_privwrite.c \
# 					lib/basic/atca_basic_verify.c \
# 					lib/basic/atca_basic_lock.c \
# 					lib/basic/atca_basic_random.c \
# 					lib/basic/atca_basic_selftest.c \
# 					lib/basic/atca_basic_kdf.c \
# 					lib/basic/atca_basic_gendig.c \
# 					lib/basic/atca_basic_aes_cbc.c \
# 					lib/basic/atca_basic_secureboot.c \
# 					lib/basic/atca_basic_aes_gcm.c \
# 					lib/basic/atca_basic_sha.c \
# 					lib/basic/atca_basic_aes_ctr.c \
# 					lib/basic/atca_helpers.c \
# 					lib/basic/atca_basic_sign.c \
# 					lib/basic/atca_basic_derivekey.c \
# 					lib/basic/atca_basic_read.c \
# 					lib/basic/atca_basic_mac.c \
# 					lib/basic/atca_basic_genkey.c \
# 					lib/basic/atca_basic_updateextra.c \
# 					lib/basic/atca_basic_checkmac.c \
# 					lib/basic/atca_basic_info.c \
# 					lib/basic/atca_basic.c \
# 					lib/basic/atca_basic_counter.c \
# 					lib/basic/atca_basic_ecdh.c \
# 					lib/basic/atca_basic_write.c \
# 					lib/basic/atca_basic_aes.c \
# 					lib/basic/atca_basic_hmac.c \
# 					lib/atcacert/atcacert_client.c \
# 					lib/atcacert/atcacert_date.c \
# 					lib/atcacert/atcacert_def.c \
# 					lib/atcacert/atcacert_host_sw.c \
# 					lib/atcacert/atcacert_host_hw.c \
# 					lib/atcacert/atcacert_pem.c \
# 					lib/atcacert/atcacert_der.c \
# 					lib/atca_execution.c \
# 					lib/atca_command.c \
# 					lib/atca_iface.c \
# 					lib/atca_device.c \
# 					test/jwt/atca_jwt_test.c \
# 					test/atca_tests_random.c \
# 					test/atca_tests_verify.c \
# 					test/atca_tests_ecdh.c \
# 					test/atca_tests_sha.c \
# 					test/atca_tests_startup.c \
# 					test/atca_test.c \
# 					test/atca_tests_kdf.c \
# 					test/atca_tests_pause.c \
# 					test/atca_tests_write.c \
# 					test/atca_tests_hmac.c \
# 					test/atca_crypto_sw_tests.c \
# 					test/atca_tests_read.c \
# 					test/atca_utils_sizes.c \
# 					test/atca_tests_sign.c \
# 					test/atca_tests_privwrite.c \
# 					test/atca_tests_counter.c \
# 					test/atca_tests_secureboot.c \
# 					test/unity_fixture.c \
# 					test/atca_tests_gendig.c \
# 					test/atca_tests_updateextra.c \
# 					test/unity.c \
# 					test/atca_tests_info.c \
# 					test/atca_tests_selftest.c \
# 					test/atcacert/test_cert_def_2_device_csr.c \
# 					test/atcacert/test_atcacert_der_integer.c \
# 					test/atcacert/test_atcacert_def.c \
# 					test/atcacert/test_atcacert_host_hw.c \
# 					test/atcacert/test_atcacert_date_runner.c \
# 					test/atcacert/test_cert_def_0_device.c \
# 					test/atcacert/test_atcacert_der_length_runner.c \
# 					test/atcacert/test_atcacert_der_ecdsa_sig_value.c \
# 					test/atcacert/test_atcacert_der_integer_runner.c \
# 					test/atcacert/test_atcacert_der_length.c \
# 					test/atcacert/test_cert_def_1_signer.c \
# 					test/atcacert/test_atcacert_def_runner.c \
# 					test/atcacert/test_atcacert_host_hw_runner.c \
# 					test/atcacert/all_atcacert_tests.c \
# 					test/atcacert/test_atcacert_client.c \
# 					test/atcacert/test_atcacert_client_runner.c \
# 					test/atcacert/test_atcacert_der_ecdsa_sig_value_runner.c \
# 					test/atcacert/test_atcacert_date.c \
# 					test/atca_tests_aes_ctr.c \
# 					test/atca_tests_aes_cbc.c \
# 					test/atca_tests_genkey.c \
# 					test/atca_tests_aes_cmac.c \
# 					test/atca_tests_aes_gcm.c \
# 					test/atca_tests_aes.c \
# 					test/atca_tests_derivekey.c \
# 					test/atca_tests_helper.c \
# 					test/atca_tests_lock.c \
# 					test/atca_tests_mac.c \
# 					test/atca_tests_otpzero.c \
# 					test/atca_tests_nonce.c
 


