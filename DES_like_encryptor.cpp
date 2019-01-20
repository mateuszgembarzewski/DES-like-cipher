#include <iostream>
#include <bitset> 
using namespace std;

const unsigned long box1[2][8]={
	{0b101, 0b010, 0b001, 0b110, 0b011, 0b100, 0b111, 0b000},
	{0b001, 0b100, 0b110, 0b010, 0b000, 0b111, 0b101, 0b011}
};
const unsigned long box2[2][8]={
	{0b100, 0b000, 0b110, 0b101, 0b111, 0b001, 0b011, 0b010},
	{0b101, 0b011, 0b000, 0b111, 0b110, 0b010, 0b001, 0b100}
	};

/* implementation of simplified DES-Type algorithm expander function
 * on page 116.
 * each round takes 6 bits of input and produces 8 bits of output
 */
unsigned long expand(unsigned long r) {
    unsigned long out = 0;
    bool i = 0;
    //First bitshift were we mask the bit and we use the logical and 
    i = (r&0b100000) == 0b100000;
    out += 128*i;
    //second bitshift
    i = (r&0b010000) == 0b010000;
    out += 64*i;
    //third bitshift - special case - 
    i = (r&0b001000) == 0b001000;
    out += 20*i;
    //fourth bitshift
    i = (r&0b000100) == 0b000100;
    out += 40*i; //40 because 4 is in 4th and 6th position (32bits + 8bits)
    //fifth bitshift
    i = (r&0b000010) == 0b000010;
    out += 2*i;
    //sixth bitshift
    i = (r&0b000001) == 0b000001;
    out += 1*i;
    
    return out;
}

/* Apply substitution-box 'box' to input 's' 
 * 's' is a four-bits input; function returns a 3-bits value
 */
unsigned long sub(const unsigned long box[][8], unsigned long s) {
    // TODO: Write this code
    unsigned long out;
    bool rowSelect;
    unsigned long colSelect;
    rowSelect = (s&0b1000) == 0b1000;
    colSelect = (s&0b0111);
    out = box[rowSelect][colSelect];
    return out;
    
}

/* Execute the f-function on r, using subkey k 
 * r is a 6-bits value; k is an b-bits subkey. Returns a 6-bits value
 */
 
unsigned long f(unsigned long r, unsigned long k) {
    unsigned long s1 = 0;
    unsigned long s2 = 0;
    unsigned long out = expand(r);
    
    out = out ^ k;
    s1 = (out&0b11110000) >> 4;
    s2 = out&0b00001111;
    s1 = sub(box1,s1);
    s2 = sub(box2,s2);
    
    out = (s1<<3)|s2;
    return out;
    
}

/* Returns an 8-bits subkey, derived from an 9-bits input key */
unsigned long subkey(unsigned long key, unsigned int n) {
	
	for (int i=0; i < n-1; i++) {
		if ((key & 0b100000000) == 0b100000000) 
			key = (key << 1) | 1;
		else
			key = key << 1;
	}

	return (key >> 1) & 0b11111111;
}

/* perform one round of encryption */
unsigned long round(unsigned long in, unsigned long key, int round) {
    unsigned long out = 0; 
    unsigned long l = (in&0b111111000000)>>6;
    unsigned long r = (in&0b000000111111);
    unsigned long r1 = r;
    out = (r<<6) | l ^ f(r1,subkey(key, round));
    return out;
    
}

/* the driver function. Do not make changes to this */
int main() {
	unsigned long plain = 0b11100100110;
	unsigned long key   = 0b010011001;

	if (expand(0b011001) != 0b01010101) {
		cerr << "Expander failed" << endl;
		return 1;
	}
	if (sub(box1, 0b1010) != 0b110) {
		cerr << "Sub1 failed" << endl;
		return 2;
	}
	if (subkey(key, 4) != 0b01100101) {
	    cerr << "Subkey derivation failed" << endl;
		return 3;
	}
	if (f(0b100110, subkey(key, 4)) != 0b000100) {
		cerr << "f failed" << endl;
		return 4;
	}
	if (round(0b011100100110, key, 4) != 0b100110011000) {
		cerr << "round failed" << endl;
		return 4;
	}

	unsigned long cipher = round(plain, key, 4);

	cout << " plain  = " << plain 
	     << "; cipher = " << cipher 
	     << " (" << bitset<12>(cipher) << ")" 
	     << endl;
}
