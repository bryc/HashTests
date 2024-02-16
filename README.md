# jsHashTest
A web-based hash/checksum testing suite, with a focus on finding obvious quality problems

# Goals
* This will be a merger of all previous hash test code into one flexible app.
* A separate .JS file will house an object, storing all the hash functions.
  * This is used to generate the list of pages dynamically.
* IndexedDB should store past test results for quick retrieval.
  * User should be able to clear database for specific hashes, such as during testing.
  * Ideally, a per-hash import/export feature so user can backup potentially useful results before clearing.
  * User should be able to cycle through past tests before having to run a new one.
  * At the very least, past results that have failed should be saved; a running count of failures (collisions) could prove useful.
* Modular test design
  * User should be able to toggle desired tests dynamically (currently this must be done by commenting out function calls).
  * App output display should adapt gracefully to handle omitted tests.
  
