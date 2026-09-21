import unittest
from src.parser import parse
from src.passes import constant_fold, fuse_mma_bias_relu, tile_matmul, lower_to_npu
from src.cost_model import estimate_gemm

class PipelineTest(unittest.TestCase):
    def test_fusion_and_lowering(self):
        f = parse('''func @main:\n  %m = npu.matmul %A, %B {M=64,N=64,K=64}\n  %a = npu.add %m, %bias\n  %r = npu.relu %a\n  return %r\n''')
        self.assertEqual(fuse_mma_bias_relu(f), 1)
        self.assertEqual(tile_matmul(f, 16, 16, 16), 1)
        self.assertEqual(lower_to_npu(f), 1)
        self.assertIn('launch_fused_gemm_bias_relu', f.format())
        self.assertIn('TM=16', f.format())

    def test_constant_fold(self):
        f = parse('''func @main:\n  %x = const 7\n  %y = const 5\n  %z = add %x, %y\n  return %z\n''')
        self.assertEqual(constant_fold(f), 1)
        self.assertIn('%z = const 12', f.format())

    def test_cost_model(self):
        e = estimate_gemm(128, 128, 128, 32, 32, 32)
        self.assertGreater(e['estimated_cycles'], 0)
        self.assertEqual(e['tiles'], 64)

if __name__ == '__main__':
    unittest.main()
