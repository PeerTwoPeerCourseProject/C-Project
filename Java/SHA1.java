import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;


public class SHA1 {
	/**
	 * SHA1 is class with static methods that are used in DHTJava.java
	 * SHA1 has methods for making SHA-1 hash and int to byte and via versa 
	 * 
	 */
	
	
	public SHA1(){
		/**
		 * SHA1 has only static methods
		 */
	}
	
	
	public static byte[] SHA_1(String str){
		/**
		 * SHA_1 makes given string to SHA-1 hash byte[]
		 * This method is used to make file name hash when Java is stating store, receive or dumb operation to DHT
		 * SHA_1 uses MessageDigest to make SHA-1 hash
		 * 
		 * Arguments:
		 * - String str is input string
		 * 
		 * Return:
		 * - hash byte[] if no exception happens
		 * - null if exception happens
		 */
		MessageDigest mesd;
		try {
			mesd = MessageDigest.getInstance("SHA-1");
			mesd.update(str.getBytes());
			byte bytes [] = mesd.digest();
			return bytes;
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
			return null;
		}
	}
	public static byte[] int_to_byte(int in){
		/**
		 * int_to_byte is integer conversion to byte[] method
		 * Method is used when communicating with DHT node by using data output stream
		 * Byte operations are used to make conversion
		 * 
		 * Arguments:
		 * - int, integer to be converted
		 * 
		 * Return:
		 * - byte[] what can be send with data output stream
		 */
		return new byte[] { (byte)(in >> 8), (byte)(in & 0xFF) };
	}
	public static int bytes_to_int(byte[] by){
		/**
		 * bytes_to_int is byte[] conversion to integer method
		 * Method is used when communicating with DHT node by using data input stram
		 * Byte operations are used to make conversion
		 * 
		 * Arguments:
		 * - byte[] to be converted
		 * 
		 * Return:
		 * - integer that is value of input byte[]
		 */
		return (by[0] & 0xFF) << 8 | by[1] & 0xFF;
	}


}
