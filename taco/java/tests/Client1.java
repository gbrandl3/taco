import fr.esrf.TacoApi.*;
import fr.esrf.TacoApi.xdr.*;

/* A very first simple client */

public class Client1 implements TacoConst {


  public static void main(String[] args) {

    try {

      TacoDevice dev1  = new TacoDevice("sr/testdev/1");
      TacoDevice dev2  = new TacoDevice("sr/testdev/2");
      TacoDevice dev3  = new TacoDevice("//aries/sr/rf-tra/tra3");

      while(true) {

        long t0 = System.currentTimeMillis();
        try {
	
          TacoData state = dev1.get(DevState); 
          short stateValue = state.extractShort();
          System.out.println("State = " + stateValue + " [" + XdrTacoType.getStateName(stateValue) + "]");
	  
	  dev1.setProtocol(TacoDevice.PROTOCOL_TCP);
	  

/*
          TacoData state2 = dev2.get(DevState);	  
          short stateValue2 = state2.extractShort();
          System.out.println("State = " + stateValue2 + " [" + XdrTacoType.getStateName(stateValue2) + "]");

          TacoData strArr = dev3.get(DevGetSigConfig);
          String[] cfgSig = strArr.extractStringArray();
          System.out.println("Config = Array of " + cfgSig.length);
*/	  	  	  
	  
        } catch (TacoException e1) {
          System.out.println("TacoException: " + e1.getErrorString());
        }
        System.out.println("Time: " + (System.currentTimeMillis() - t0) + " ms");

        try { Thread.sleep(2000); } catch (Exception e) {};

      }

      //TacoData setPoint = new TacoData();
      //setPoint.insertFloat(200.5f);
      //dev.put(TacoConst.DevSetValue,setPoint);
      //dev.free();

    } catch(TacoException e2) {

      System.out.println("TacoException: " + e2.getErrorString());
      System.out.println("\n-----------------------\n");
      e2.printStackTrace();

    }

  }


}
