import DEVCMDS
import TACOCommands
import StarterCommands
import TACOClient
from TACOClient import TACOError

class Starter (TACOClient.Client):
        def __init__( self, name):
                TACOClient.Client.__init__( self, name)

        def run(self, input):
		self.execute(DEVCMDS.DevRun, input)

        def stop(self, input):
		self.execute(DEVCMDS.DevStop, input)

        def restore(self, input):
		self.execute(DEVCMDS.DevRestore, input)

        def startAll(self, input):
		self.execute(StarterCommands.DevStartAll, input)

        def stopAll(self, input):
		self.execute(StarterCommands.DevStopAll, input)

        def getRunningServers(self, input):
		return self.execute(StarterCommands.DevGetRunningServers, input)

        def getStoppedServers(self, input):
		return self.execute(StarterCommands.DevGetStoppedServers, input)

        def start(self, input) :
		self.execute(StarterCommands.DevStart, input)

        def readLog(self, input) : 
                return self.execute(StarterCommands.DevReadLog, input)

        def updateServerInfo(self) :
		self.execute(StarterCommands.UpdateServerInfo)

