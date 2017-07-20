package com.java.classes;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;
import java.util.Arrays;
import java.util.Base64;


import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;

//contains cryptographic methods that hash user passwords and check them against stored hash
//properties can adjust the strength of the hash, currently set at default
public class PasswordHasher {

  private static final String ALGORITHM = "PBKDF2WithHmacSHA1";
  private static final int SIZE = 128;
  private  SecureRandom random;
  private  int cost = 16;

  public PasswordHasher()
  {
	  random = new SecureRandom();
  }

  //returns hash of given password
  public String hash(char[] password)
  {
    byte[] salt = new byte[SIZE / 8];
    random.nextBytes(salt);
    byte[] dk = pbkdf2(password, salt, 1 << cost);
    byte[] hash = new byte[salt.length + dk.length];
    System.arraycopy(salt, 0, hash, 0, salt.length);
    System.arraycopy(dk, 0, hash, salt.length, dk.length);
    Base64.Encoder enc = Base64.getUrlEncoder().withoutPadding();
    return enc.encodeToString(hash);
  }

  private static int iterations(int cost)
  {
    if ((cost & ~0x1F) != 0)
      throw new IllegalArgumentException("cost: " + cost);
    return 1 << cost;
  }
  
  //verifies given password matches given token
  public boolean authenticate(char[] password, String token)
  {
	int iterations = iterations(cost);
    byte[] hash = Base64.getUrlDecoder().decode(token);
    byte[] salt = Arrays.copyOfRange(hash, 0, SIZE / 8);
    byte[] check = pbkdf2(password, salt, iterations);
    int zero = 0;
    for (int idx = 0; idx < check.length; ++idx)
      zero |= hash[salt.length + idx] ^ check[idx];
    return zero == 0;
  }

  private static byte[] pbkdf2(char[] password, byte[] salt, int iterations)
  {
    KeySpec spec = new PBEKeySpec(password, salt, iterations, SIZE);
    try {
      SecretKeyFactory f = SecretKeyFactory.getInstance(ALGORITHM);
      return f.generateSecret(spec).getEncoded();
    }
    catch (NoSuchAlgorithmException ex) {
      throw new IllegalStateException("Missing algorithm: " + ALGORITHM, ex);
    }
    catch (InvalidKeySpecException ex) {
      throw new IllegalStateException("Invalid SecretKeyFactory", ex);
    }
  }

}
