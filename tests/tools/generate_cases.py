import hashlib
import os
import random

target_directory = "tests/files"

if __name__ == '__main__':
    characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
    algorithms = ["md5", "sha1", "sha224", "sha256", "sha384", "sha512", "sha3_224", "sha3_256", "sha3_384", "sha3_512"]
    for algorithm_name in algorithms:
        for _ in range(8):
            content = b''
            for i in range(random.randint(1, 100)):
                content += ''.join(random.choices(characters, k=random.randint(0, len(characters)))).encode()
            algorithm = hashlib.new(algorithm_name, content)
            hex_digest = algorithm.hexdigest()
            sub_directory = os.path.normpath(os.path.join(target_directory, algorithm_name))
            if not os.path.exists(sub_directory):
                os.makedirs(sub_directory)
            with open(os.path.join(sub_directory, hex_digest), 'wb') as f:
                f.write(content)