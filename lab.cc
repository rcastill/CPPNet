#include <iostream>
#include "include/net/bytebuffer.h"
#include "include/net/address.h"

using namespace std;

int main(int argc, char **argv) {
    ByteBuffer bb(sizeof(int) * 5);
    Address address(127, 0, 0, 1, 59848);

    cout << bb.PutInt(123) << endl;
    cout << bb.PutInt(7520) << endl;
    cout << bb.PutInt(8388607) << endl;
    cout << bb.PutInt(1928951800) << endl;
    cout << bb.PutInt(213124141) << endl;

    bb.Rewind();

    for (int i = 0; i < 5; i++) {
        int n = bb.GetInt();
        cout << n << endl;
    }

    int h = 8388608;
    u_long l = htonl((u_long) h);
    int test = (int) (u_long) ntohl((u_long) (int) l);
    cout << "test " << test << endl;

    /*int helloThere[2];

    ByteBuffer buff(helloThere, sizeof(helloThere));
    buff.TakeCareOfCopies();
    buff.DisableDestructor();
    buff.PutInt(42);
    buff.PutInt(5428);

    buff.Rewind();

    int i1 = buff.GetInt();
    int i2 = buff.GetInt();

    int *butt = (int *) buff.GetBufferCopy();

    cout << i1 << " / " << butt[0] << endl;
    cout << i2 << " / " << butt[1] << endl;

    /*ByteBuffer bb(sizeof(int) * 2);
    bb.PutInt(42);
    bb.PutInt(5428);

    int *intbuff = (int *) bb.GetBuffer();

    for (int i = 0; i < 2; i++)
        cout << intbuff[i] << endl;*/

    //cout << i << " / " << s << endl;
    return 0;
}