import fr.esrf.TacoApi.*;
import fr.esrf.TacoApi.xdr.*;

/* A very first simple client */

public class TestInstVerify implements TacoConst {


  public static void main(String[] args) {

    try {

      TacoDevice dev  = new TacoDevice("test/device/1");

      while(true) {

        long t0 = System.currentTimeMillis();
        try {
	
          TacoData state = dev.get(DevState); 
          short stateValue = state.extractShort();
          System.out.println("State = " + stateValue + " [" + XdrTacoType.getStateName(stateValue) + "]");
	  
	  dev.setProtocol(TacoDevice.PROTOCOL_TCP);
	  

        } catch (TacoException e1) {
          System.out.println("TacoException: " + e1.getErrorString());
        }
        System.out.println("Time: " + (System.currentTimeMillis() - t0) + " ms");

        try { Thread.sleep(2000); } catch (Exception e) {};

      }

      TacoData setPoint = new TacoData();
      setPoint.insertFloat(200.5f);
      dev.put(TacoConst.DevSetValue,setPoint);
      dev.free();

    } catch(TacoException e2) {

      System.out.println("TacoException: " + e2.getErrorString());
      System.out.println("\n-----------------------\n");
      e2.printStackTrace();

    }

  }


}
