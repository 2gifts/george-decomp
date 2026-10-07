import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from analyze import validated_elf
from trace_allocator_locks import AllocatorLockTrace, RANGES, STUBS, OWNER, COUNT, SEMAPHORE, RETURN, make_fixture, fixtures


class LockGuards(unittest.TestCase):
    def test_memory_extent_alignment_and_initialization(self):
        t = AllocatorLockTrace(b'')
        for address, size in ((COUNT + 1, 4), (COUNT, 8), (0x80001, 4), (0x12340, 4)):
            with self.assertRaises(ValueError):
                t.load(address, size)
            with self.assertRaises(ValueError):
                t.save(address, 0, size)
        with self.assertRaisesRegex(ValueError, 'uninitialized'):
            t.load(COUNT, 4)

    def test_encoding_and_syscall_guards(self):
        t = AllocatorLockTrace(b'')
        for instruction in (0x0C000000, 0x2404002F, 0x8C6259F8, 0x44810000, 0x00800008):
            with self.assertRaisesRegex(ValueError, 'unsupported'):
                t.execute(instruction, RANGES[0][0])
        with self.assertRaisesRegex(ValueError, 'unreviewed syscall'):
            t.execute(12, RANGES[0][0])
        with self.assertRaisesRegex(ValueError, 'wrong syscall'):
            t.execute(12, STUBS[0][0] + 4)

    def test_fetch_and_entry_bounds(self):
        t = AllocatorLockTrace(b'')
        for pc in (RANGES[0][0] - 4, RANGES[0][1], STUBS[0][0] + 16, RANGES[0][0] + 1):
            with self.assertRaisesRegex(ValueError, 'unreviewed'):
                t.fetch(pc)
        with self.assertRaisesRegex(ValueError, 'entry/stop'):
            t.run(RANGES[1][0])
        for routine, thread, mutation in ((True, 0, 0), (2, 0, 0), (0, -1, 0), (0, 0, 16)):
            with self.assertRaisesRegex(ValueError, 'observer input'):
                AllocatorLockTrace(b'', routine, thread, mutation)
        with self.assertRaisesRegex(ValueError, 'fixture word'):
            make_fixture(b'', 0, 0, 0, 0x100000000, 0, 0)

    def test_control_in_delay_and_outside_body(self):
        class Controlled(AllocatorLockTrace):
            def fetch(self, pc):
                return self.code.get(pc, 0)
        a = RANGES[0][0]
        t = Controlled(b'')
        t.code = {a: 0x10000000, a + 4: 0x03E00008}
        with self.assertRaisesRegex(ValueError, 'control in delay'):
            t.run(a)
        t = Controlled(b'')
        t.code = {a: 0x10000040}
        with self.assertRaisesRegex(ValueError, 'outside body'):
            t.run(a)
        t = Controlled(b'')
        t.code = {a: 0x1000FFFF}
        with self.assertRaisesRegex(ValueError, 'instruction bound'):
            t.run(a)

    def test_real_terminal_and_stub_order(self):
        class Controlled(AllocatorLockTrace):
            def fetch(self, pc):
                return 0x03E00008 if pc == RANGES[0][0] else 0
        t = Controlled(b'')
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'stub return'):
            t.run(RANGES[0][0])
        t = AllocatorLockTrace(b'', 1)
        t.r[3] = 47
        with self.assertRaisesRegex(ValueError, 'order'):
            t.execute(12, STUBS[0][0] + 4)


class LockOriginal(unittest.TestCase):
    def setUp(self):
        path = ROOT / 'orig/SLUS_216.68'
        if not path.is_file():
            self.skipTest('private original absent')
        _, self.original = validated_elf(path)

    def test_post_wait_capture_and_fresh_count(self):
        c = make_fixture(self.original, 0, 0x80000001, 0, 2, 0xFFFFFFFF, 4)
        self.assertEqual((c['expected_owner'], c['expected_count']), (0x80000001, 0))
        self.assertEqual(c['events'][5:10], [1, 0xFFFFFFFF, 2, 0, 0xFFFFFFFF])

    def test_get_callback_controls_fresh_owner_and_semaphore(self):
        c = make_fixture(self.original, 0, 0, 0, 0, 0, 1)
        self.assertEqual(c['events'][5:10], [1, 0x80000003, 0x7FFFFFFF, 1, 0x80000003])
        c = make_fixture(self.original, 0, 0xFFFFFFFF, 0, 0, 0, 2)
        self.assertEqual((c['event_words'], c['expected_count']), (5, 0))

    def test_release_publication_order_and_unchecked_wrap(self):
        c = make_fixture(self.original, 1, 7, 123, 1, 0x80000002, 8)
        self.assertEqual(c['events'][:5], [2, 0x80000002, 0, 0xFFFFFFFF, 0x80000002])
        self.assertEqual((c['expected_count'], c['expected_owner']), (0x80000000, 0x55667788))
        c = make_fixture(self.original, 1, 7, 123, 0, 4, 0)
        self.assertEqual((c['event_words'], c['expected_count'], c['expected_owner']), (0, 0xFFFFFFFF, 123))

    def test_full_selected_and_actual_stub_coverage(self):
        cases = fixtures(self.original)
        self.assertEqual(len(cases), 5760)
        for start, end in (*RANGES, *((a, a + 16) for a, _ in STUBS)):
            self.assertEqual({pc for c in cases for pc in c['visited'] if start <= pc < end}, set(range(start, end, 4)))
        outcomes = {pc: {taken for c in cases for p, taken in c['branches'] if p == pc}
                    for pc in (0x396708, 0x39675C)}
        self.assertEqual(list(outcomes.values()), [{False, True}, {False, True}])


if __name__ == '__main__':
    unittest.main()
