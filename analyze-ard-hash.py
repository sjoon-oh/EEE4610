#
# author: SukJoon Oh, sjoon.oh.dev@pm.me
# 21.03.23.

import json
import argparse
import ast

import os

banner = \
"""author: SukJoon Oh, sjoon.oh.dev@pm.me
Amazon Review Dataset Processing Script"""

print(banner)

def register_parser():
    parser = argparse.ArgumentParser(description=banner)    

    parser.add_argument('--filename', type=str,
        dest='fn',
        help='Provide the metadata filename.',
        default=''
    )

    # Attributes to remove from metadata dict form.
    parser.add_argument('--rm-key', nargs='+',
        dest='mrk', help='Set an attribute to delete.'
    )

    #
    # parser.add_argument('--expt-dict', nargs='+',
    #     dest='expt', 
    # )

    return parser


import hashlib
def get_hash_value(in_str, in_digest_bytes_size=4, in_return_type='number'):

    assert 1 <= in_digest_bytes_size and in_digest_bytes_size <= 64
    blake  = hashlib.blake2b(in_str.encode('utf-8'), digest_size=in_digest_bytes_size)
    if in_return_type == 'hexdigest': return blake.hexdigest()
    elif in_return_type == 'number': return int(blake.hexdigest(), base=16)
    return blake.digest()


def analyze_line(single_line, stat):
    # _ = {
    #     'keys': ['key1', 'key2', 'key3', ...],
    #     'data': {
    #       'key1': {'count': 10, 'inst': ['instance1', 'instance2', ...].
    #       'key2': {'count': 10, 'inst': ['instance1', 'instance2', ...] 
    #   } }
    # }
    _ = single_line
    for key in _.keys():
        
        # First, check if the value is not in a dict form.            
        if isinstance(single_line[key], dict):

            subkeys = list(single_line[key].keys())
            for subkey in subkeys:
                single_line[key]['%s/%s' % (key, subkey)] = single_line[key][subkey]
                del single_line[key][subkey]

            analyze_line(single_line[key], stat) # recursive call

        else:
            if key not in stat['keys']: # Case when element is not in dict form.
                stat['keys'].append(str(key)) # register
                stat['data'][str(key)] = \
                    { 'count': 1, 'inst': [ get_hash_value(str(_[str(key)])) ]} # case when new detected

                # print(type(get_hash_value(str(_[str(key)]))))
            else: 
                if _[str(key)] in stat['data'][str(key)]['inst']:
                    pass
                else:
                    stat['data'][str(key)]['count'] += 1
                    stat['data'][str(key)]['inst'].append(
                            # int.from_bytes(get_hash_value(str(_[str(key)])), 'little')
                            get_hash_value(str(_[str(key)]))
                        )

    # print(stat)
    return stat

# 
# Main
if __name__ == '__main__':
    parser = register_parser()
    args = parser.parse_args()

    line_count = 1;
    total_lines = int(os.popen('wc -l %s' % args.fn).read().split()[0])

    print('Processing %s ...' % args.fn)

    import pathlib
    if os.path.isfile(str(pathlib.Path().absolute()) + '/' + args.fn.replace('.json', '-p1')):
        print('File exists.')

    else:
        # Remove unnecessary attributes for both dataset and metadata
        if args.fn != '':
            with open('%s%s' % (args.fn.replace('.json', ''), '-p1'), 'w') as write_data:
                with open(args.fn, 'r') as read_data:
                    for line in read_data: 
                        _ = json.loads(line)
                        #_ = ast.literal_eval(line)
                        
                        # Remove unnecessary traits
                        for attr in args.mrk:
                            if attr in _.keys(): del _[attr]

                        write_data.write(str(_) + '\n')
                        
                        print('\rStatus: %7.2f%s' % ((line_count / total_lines * 100), '%'), end='')
                        line_count += 1

    print('\nDone processing %s' % args.fn)

    print('Counting attributes:')
    filename = args.fn.replace('.json', '-p1')

    

    print('Processing %s ...' % filename)

    stat = {
        'keys': [],
        'data': {}
    }

    line_count = 1;
    # total_lines = int(os.popen('wc -l %s' % filename).read().split()[0])
    
    # This is where the collection information gathered.
    if filename != '':
        with open(filename, 'r') as read_data:
                        
            for line in read_data:
               
                _ = ast.literal_eval(line) # Single line
                analyze_line(_, stat) # record

                print('\rStatus: %5.2f%s' % ((line_count / total_lines * 100), '%'), end='')
                line_count += 1

            print('')
        
        # Generating report
        with open('{}-report.txt'.format(filename), 'w') as write_file:

            write_file.write('\nAmazon Review Data: %s\n' % args.fn)
            write_file.write('Keys %s in total:\n' % (len(stat['keys'])))
            write_file.write(' - : %s\n' % str(stat['keys']))
            write_file.write('Statistics:\n')
            
            for key in stat['keys']:
                write_file.write(' - %s: %s\n' % (key, stat['data'][key]['count']))
            
    
    print('Processing done. Bye!!')
