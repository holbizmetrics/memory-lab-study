import json
import unittest
import numpy as np

import memory_dsp as dsp


def param(kind):
    return {'kind':kind,'bias':.04 if kind=='biased' else 0,
            'depth':.7 if kind=='sag' else 0,'attack_ms':8,'release_ms':150}


class DSPTests(unittest.TestCase):
    def test_invalid_or_executable_parameters_rejected(self):
        for expression in ('__import__("os")', '{"kind":"static"}',
                           json.dumps({**param('sag'),'depth':True}),
                           json.dumps({**param('biased'),'bias':2})):
            with self.assertRaises((ValueError, TypeError)):
                dsp.parameters(expression,'v3' if 'sag' in expression else 'v2')

    def test_zero_drive_and_source_preservation(self):
        x=np.linspace(-.5,.5,1000);copy=x.copy()
        for kind in ('static','biased','sag'):
            self.assertTrue(np.array_equal(x,dsp.process(x,48000,0,param(kind))))
            dsp.process(x,48000,12,param(kind))
            self.assertTrue(np.array_equal(x,copy))

    def test_declared_memory_properties(self):
        self.assertEqual(0,dsp.memory_probe(param('static')))
        self.assertEqual(0,dsp.memory_probe(param('biased')))
        self.assertGreater(dsp.memory_probe(param('sag')),1e-7)

    def test_bias_zero_input_is_zero(self):
        self.assertTrue(np.array_equal(dsp.process(np.zeros(1000),48000,12,param('biased')),np.zeros(1000)))

    def test_bounded_grid_and_silence(self):
        rng=np.random.default_rng(42)
        for kind in ('static','biased','sag'):
            for drive in dsp.STRENGTHS:
                for level in (.02,.3,2.):
                    y=dsp.process(level*rng.normal(size=2000),48000,drive,param(kind))
                    self.assertTrue(np.all(np.isfinite(y)))
                    self.assertLess(np.max(np.abs(y)),2.)
            self.assertTrue(np.array_equal(dsp.process(np.zeros(1000),48000,5,param(kind)),np.zeros(1000)))

    def test_export_chain_matching(self):
        source=.2*np.sin(2*np.pi*220*np.arange(4800)/48000)
        for kind in ('static','biased','sag'):
            raw,matched,gain=dsp.render(source,48000,5,param(kind))
            self.assertLess(abs(dsp.metrics(matched,48000)['rms_dbfs']+24),1e-8)
            self.assertLess(np.max(np.abs(matched)),.98)
            self.assertEqual(len(source),len(raw))


if __name__=='__main__':unittest.main()
