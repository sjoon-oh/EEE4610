#
# author: SukJoon Oh, sjoon.oh.dev@pm.me
# 21.03.23.

import json
import argparse
import ast

banner = \
"""author: SukJoon Oh, sjoon.oh.dev@pm.me
Amazon Review Dataset Processing Script"""

print(banner)

def register_parser():
    parser = argparse.ArgumentParser(description=banner)

    parser.add_argument('--dataset', type=str, 
        dest='d_filename',
        help='Provide the dataset filename.',
        default=''
    )

    # Attributes to remove from dataset dict form.
    parser.add_argument('--dataset-rm-key', nargs='+',
        dest='drk', help='Set an attribute to delete.'
    )
    

    parser.add_argument('--metadata', type=str,
        dest='m_filename',
        help='Provide the metadata filename.',
        default=''
    )

    # Attributes to remove from metadata dict form.
    parser.add_argument('--metadata-rm-key', nargs='+',
        dest='mrk', help='Set an attribute to delete.'
    )

    return parser

if __name__ == '__main__':
    parser = register_parser()
    args = parser.parse_args()

    print('Processing %s ...' % args.d_filename)

    # Remove unnecessary attributes for both dataset and metadata
    with open('%s%s' % (args.d_filename.replace('.json', ''), '-p1'), 'w') as write_data:
        with open(args.d_filename, 'r') as read_data:
            for line in read_data: 
                # _ = json.loads(line)
                _ = ast.literal_eval(line)
                
                # Remove unnecessary traits
                for attr in args.drk:
                    if attr in _.keys(): del _[attr]

                write_data.write(str(_) + '\n')

    print('done. \nProcessing %s ...' % args.m_filename)

    with open('%s%s' % (args.m_filename.replace('.json', ''), '-p1'), 'w') as write_data:
        with open(args.m_filename, 'r') as read_data:
            for line in read_data: 
                
                # Slight modification!
                _ = ast.literal_eval(line)
            
                # Remove unnecessary traits
                for attr in args.mrk:
                    if attr in _.keys(): del _[attr]

                write_data.write(str(_) + '\n')

    print('done.')

    print('Counting attributes:')

    file_names = [
        args.d_filename.replace('.json', '-p1'),
        args.m_filename.replace('.json', '-p1')    
    ]

    import os

    for filename in file_names:

        print('Processing %s ...' % filename)

        stat = {
            'keys': [],
            'data': {}
        }

        line_count = 1;
        total_lines = int(os.popen('wc -l %s' % filename).read().split()[0])
        
        # This is where the collection information gathered.
        with open(filename, 'r') as read_data:
                        
            for line in read_data:
                # _ = {
                #     'keys': ['key1', 'key2', 'key3', ...],
                #     'data': {
                #       'key1': {'count': 10, 'inst': ['instance1', 'instance2', ...].
                #       'key2': {'count': 10, 'inst': ['instance1', 'instance2', ...] 
                #   } }
                # }
                
                _ = ast.literal_eval(line) # Single line

                for key in _.keys():
                    if key not in stat['keys']:
                        stat['keys'].append(str(key)) # register
                        stat['data'][str(key)] = \
                            { 'count': 1, 'inst': [ _[str(key)] ]} # case when new detected
                    else: 
                        if _[str(key)] in stat['data'][str(key)]['inst']:
                            pass
                        else:
                            stat['data'][str(key)]['count'] += 1
                            stat['data'][str(key)]['inst'].append(_[str(key)])

                    # print(json.dumps(stat, indent=4))
                print('\r%3.2f percent' % (line_count / total_lines * 100), end='')
                line_count += 1
                # if line_count >= total_lines:
                #    print('=', end='')
                #    total_lines -= int(total_lines * 0.1)
            print('')
        
        # Generating report
        with open('{}-report.txt'.format(filename), 'w') as write_file:

            write_file.write('Amazon Review Data: %s\n' % args.d_filename)
            write_file.write('Keys %s in total:\n' % (len(stat['keys'])))
            write_file.write(' - : %s\n' % str(stat['keys']))
            write_file.write('Statistics:\n')
            
            for key in stat['keys']:
                write_file.write(' - %s: %s\n' % (key, stat['data'][key]['count']))
        
    
    print('Processing done. Bye!!')








