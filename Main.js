const express = require("express");
const { killswitch } = require("./killswitch");
app = new express();
initialized = false;
if (!initialized) {
	// Code to execute only once
	initialized = true;
}

app.listen(3001, () => {
	console.log("Server is running on port 3001");
	NOTHING();
	// ApiServices();
	// killswitch(); // Uncomment if you want to call the killswitch function
});

// const execute = () => {
// 	/**
// 	 *^C91379490.318998
//      system components are essential for the basic functionality of a computer system
//      DIMMS (memory modules)
//      motherboards
//      DL --> double layers
//      molex connectors
//      RAIDS 0
//      RAID 0
//      LCD --> pixels
//      native resolution: 4080p
//      multimeter
//      quaranting the system
//      hard disk partitions
//      white streaks within methodology
//      switches --> nintendo switch ;)
// 	 */
// 	// buy me a PC. (GPU's only)
killswitch();
// };

NOTHING = () => {
	app.listen(3001, () => {
		// ApiServices();
		console.table(new Date())();
	});
};

// const ApiServices = async () => {
// 	const client = new CovalentClient();
// 	const resp = await client.NftService.getNftsForAddress("eth-mainnet");
// 	console.log(resp.data);
// };
