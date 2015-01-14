#include <iostream>
#include "include/net/bytebuffer.h"

using namespace std;

int main(int argc, char **argv) {
    int helloThere[2];

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